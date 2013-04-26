#define PI 3.14159265358979f
#define MAX_INT 4294967295.0f
// ------------------------------ RNG ------------------------------
/*
  Part of MWC64X by David Thomas, dt10@imperial.ac.uk
  This is provided under BSD, full license is with the main package.
  See http://www.doc.ic.ac.uk/~dt10/research
*/

// Pre: a<M, b<M
// Post: r=(a+b) mod M
ulong MWC_AddMod64(ulong a, ulong b, ulong M)
{
  ulong v=a+b;
  if( (v>=M) || (v<a) )
    v=v-M;
  return v;
}

// Pre: a<M,b<M
// Post: r=(a*b) mod M
// This could be done more efficently, but it is portable, and should
// be easy to understand. It can be replaced with any of the better
// modular multiplication algorithms (for example if you know you have
// double precision available or something).
ulong MWC_MulMod64(ulong a, ulong b, ulong M)
{	
  ulong r=0;
  while(a!=0){
    if(a&1)
      r=MWC_AddMod64(r,b,M);
    b=MWC_AddMod64(b,b,M);
    a=a>>1;
  }
  return r;
}


// Pre: a<M, e>=0
// Post: r=(a^b) mod M
// This takes at most ~64^2 modular additions, so probably about 2^15 or so instructions on
// most architectures
ulong MWC_PowMod64(ulong a, ulong e, ulong M)
{
  ulong sqr=a, acc=1;
  while(e!=0){
    if(e&1)
      acc=MWC_MulMod64(acc,sqr,M);
    sqr=MWC_MulMod64(sqr,sqr,M);
    e=e>>1;
  }
  return acc;
}

uint2 MWC_SkipImpl_Mod64(uint2 curr, ulong A, ulong M, ulong distance)
{
  ulong m=MWC_PowMod64(A, distance, M);
  ulong x=curr.x*(ulong)A+curr.y;
  x=MWC_MulMod64(x, m, M);
  return (uint2)((uint)(x/A), (uint)(x%A));
}

uint2 MWC_SeedImpl_Mod64(ulong A, ulong M, uint vecSize, uint vecOffset, ulong streamBase, ulong streamGap)
{
  // This is an arbitrary constant for starting LCG jumping from. I didn't
  // want to start from 1, as then you end up with the two or three first values
  // being a bit poor in ones - once you've decided that, one constant is as
  // good as any another. There is no deep mathematical reason for it, I just
  // generated a random number.
  enum{ MWC_BASEID = 4077358422479273989UL };
	
  ulong dist=streamBase + (get_global_id(0)*vecSize+vecOffset)*streamGap;
  ulong m=MWC_PowMod64(A, dist, M);
	
  ulong x=MWC_MulMod64(MWC_BASEID, m, M);
  return (uint2)((uint)(x/A), (uint)(x%A));
}

//! Represents the state of a particular generator
typedef struct{ uint x; uint c; } mwc64x_state_t;

enum{ MWC64X_A = 4294883355U };
enum{ MWC64X_M = 18446383549859758079UL };

void MWC64X_Step(mwc64x_state_t *s)
{
  uint X=s->x, C=s->c;
	
  uint Xn=MWC64X_A*X+C;
  uint carry=(uint)(Xn<C); // The (Xn<C) will be zero or one for scalar
  uint Cn=mad_hi(MWC64X_A,X,carry);  
	
  s->x=Xn;
  s->c=Cn;
}

void MWC64X_Skip(mwc64x_state_t *s, ulong distance)
{
  uint2 tmp=MWC_SkipImpl_Mod64((uint2)(s->x,s->c), MWC64X_A, MWC64X_M, distance);
  s->x=tmp.x;
  s->c=tmp.y;
}

void MWC64X_SeedStreams(mwc64x_state_t *s, ulong baseOffset, ulong perStreamOffset)
{
  uint2 tmp=MWC_SeedImpl_Mod64(MWC64X_A, MWC64X_M, 1, 0, baseOffset, perStreamOffset);
  s->x=tmp.x;
  s->c=tmp.y;
}

//! Return a 32-bit integer in the range [0..2^32)
uint MWC64X_NextUint(mwc64x_state_t *s)
{
  uint res=s->x ^ s->c;
  MWC64X_Step(s);
  return res;
}

// ------------------------------ UTIL ------------------------------
// estimation de pi
__kernel void EstimatePi(__global int *n,
			 __global int *baseOffset,
			 __global int *acc)
{
  mwc64x_state_t rng;
  ulong samplesPerStream=(*n)/get_global_size(0);
  MWC64X_SeedStreams(&rng, (*baseOffset), 2*samplesPerStream);
    uint count=0;
    for(uint i=0;i<samplesPerStream;i++){
        ulong x=MWC64X_NextUint(&rng);
        ulong y=MWC64X_NextUint(&rng);
        ulong x2=x*x;
        ulong y2=y*y;
        if(x2+y2 >= x2)
            count++;
    }
  acc[get_global_id(0)] = count;
}

// normalisation vers (0,1] et transformation de Box-Muller
void normalisation_bm(uint u1, uint u2, float *x1, float *x2) {
  uint _u1 = u1;
  uint _u2 = u2;
  float norm_u2 = (float) (_u2 / MAX_INT);
  float norm_u1 = (float) (_u1 / MAX_INT);

  float r = native_sqrt(-2.0f * log(norm_u1));
  float phi = 2.0f * PI * ((float) norm_u2);
  *x1 = r * native_cos(phi);
  *x2 = r * native_sin(phi);
}

// pareil mais ne retourne qu'une seule v.a
float fast_norm_bm(uint u1, uint u2) {
  uint _u1 = u1;
  uint _u2 = u2;
  float norm_u1 = (float) (_u1 / MAX_INT);
  float norm_u2 = (float) (_u2 / MAX_INT);
  float r = native_sqrt(-2.0f * log(norm_u1));
  float phi = 2.0f * PI * ((float) norm_u2);
  return r * native_cos(phi);
}

// test tirages cd gaussiennes
__kernel void distribution_gauss(__global float *TIRAGES, __global int *moitie)
{
  mwc64x_state_t rng;
  ulong samplesPerStream=((*moitie) * 2)/get_global_id(0);
  MWC64X_SeedStreams(&rng, get_global_id(0), 2 * samplesPerStream);
  float u1 = 0.0f;
  float u2 = 0.0f;
  normalisation_bm(MWC64X_NextUint(&rng), MWC64X_NextUint(&rng), &u1, &u2);
  TIRAGES[get_global_id(0)] = u1;
  TIRAGES[get_global_id(0) + (*moitie)] = u2;
}

__kernel void calcul_trajectoires2(__global const float *RENDEMENTS,
                                   __global const float *VOLS,
                                   __global const float *TI,
                                   __global float *TIRAGES,
                                   __constant float *rendement,
                                   __constant int *nb_actions,
                                   __constant int *horizon,
				   __constant int *nb_tirages) {
  TIRAGES[get_global_id(0)] = 0;
  float tmp;
  mwc64x_state_t rng;
  ulong samplesPerStream=(*horizon)*(*nb_actions);
  MWC64X_SeedStreams(&rng, 0, samplesPerStream);
  for(int a = 0; a < (*nb_actions); a++) {
    tmp = RENDEMENTS[a];
    for(int t = 1; t <= (*horizon); t++) {
      tmp = tmp * exp((TI[a] - ((VOLS[a] * VOLS[a]) / 2) * 1 + VOLS[a] * fast_norm_bm(MWC64X_NextUint(&rng), MWC64X_NextUint(&rng))));
    }
    TIRAGES[get_global_id(0)] += tmp;
  }
  TIRAGES[get_global_id(0)] = TIRAGES[get_global_id(0)] - (*rendement);
}

__kernel void calcul_trajectoires(__global const float *RENDEMENTS,
				  __global const float *VOLS,
				  __global const float *TI,
				  __global const float *ALEA,
                                  __global float *rendement,
				  __global float *TIRAGES,
				  __constant int *nb_actions,
				  __constant int *horizon) {
  int i = get_global_id(0);
  TIRAGES[i] = 0;
  float tmp = 0;
  for(int a = 0; a < (*nb_actions); a++) {
    tmp = RENDEMENTS[a];
    for(int t = 1; t <= (*horizon); t++) {
      int index = i * (*nb_actions);
      index += a * (*horizon);
      index += t;
      tmp = tmp*exp(TI[a] * 1 + VOLS[a] * 1 * ALEA[index]);
    }
    TIRAGES[i]+=tmp;
  }
  TIRAGES[i] = (*rendement) - TIRAGES[i];
}
