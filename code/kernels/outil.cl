__kernel void calcul_variance(__global const float *TIRAGES,
	      			  __global float *nb_value,      	 
				  __global float *CARRE, 		  /*tableau de taille nombre processeur */
				  __global float *ESPERANCE){		  /*tableau de taille nombre processeur */
  int i = get_global_id(0)*(*nb_value); // offset
  float tmp_esp;
  float tmp_carre;
  for(int t = 1; t <= (*nb_value); t++) {
    tmp_esp=tmp_esp+TIRAGES[i+t];
    tmp_carre=tmp_carre+TIRAGES[i+t]*TIRAGES[i+t];
  }
  CARRE[get_global_id(0)]=tmp_carre;
  ESPERANCE[get_global_id(0)]=tmp_esp;
}
