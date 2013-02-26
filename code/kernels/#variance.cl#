__kernel void calcul_variance(__global const float *TIRAGES,
	      			  __global const int *nb_Simulation,
	      			  __global const int *nb_value_par_thread, 
	      			  __global float *esperance,
	      			  __global float *variance,
	      			  __global const int *nb_THREAD,        	 		 
				  __global float *ESPERANCE)     /* tableau de taille nombre processeur  */
{		  
  int i = get_global_id(0)*(*nb_value_par_thread);
  float tmp=0.0;
  for(int t = 0; t < (*nb_value_par_thread); t++) {
    tmp=tmp+ (TIRAGES[i+t] - (*esperance))*(TIRAGES[i+t] - (*esperance)) ;
  }
  ESPERANCE[get_global_id(0)]=tmp;


  barrier(CLK_LOCAL_MEM_FENCE);

  if(get_global_id(0) == 0){
    float esperance_tmp=0.0;
    for(int j=0; j < (*nb_THREAD)-2 ; j++ ){
      esperance_tmp = esperance_tmp + ESPERANCE[j]/ ((*nb_Simulation-1)*1.0);
      ESPERANCE[j]=esperance_tmp;
    }
    *esperance=esperance_tmp;
  }
}
