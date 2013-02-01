__kernel void prototype(__global const float *PORTEFEUILLE,
			__global const float *ALEA,
			__global float *TIRAGES,
			__global int *nb_actions,
			__global int *horizon) {
  int i = get_global_id(0);
  TIRAGES[i] = 0;
  float tmp;
  for(int a=0; a < (*nb_actions);a++) {
    tmp = PORTEFEUILLE[a];
    for(int t=0; t < (*horizon);t++) {
      int index = i * (*nb_actions);
      index += a * (*horizon);
      index += t;
      tmp = tmp*ALEA[index];
    }
    TIRAGES[i]+=tmp;
  }
  tmp = 0;
  for(int j=0; j < (*nb_actions); j++) {
    tmp += PORTEFEUILLE[j];
  }
  TIRAGES[i]+=tmp;
}
