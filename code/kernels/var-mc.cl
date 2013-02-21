__kernel void calcul_trajectoires(__global const float *RENDEMENTS,
				  __global const float *VOLS,
				  __global const float *TI,
				  __constant float *ALEA,
				  __global float *TIRAGES,
				  __constant int *nb_actions,
				  __constant int *horizon) {
  int i = get_global_id(0);
  TIRAGES[i] = 0;
  float tmp;
  for(int a = 0; a < (*nb_actions); a++) {
    tmp = RENDEMENTS[a];
    for(int t = 1; t <= (*horizon); t++) {
      int index = i * (*nb_actions);
      index += a * (*horizon);
      index += t;
      tmp = tmp + TI[a] * 1 + VOLS[a] * 1 * ALEA[index];
    }
    TIRAGES[i]+=tmp;
  }
}
