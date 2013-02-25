__kernel void calcul_variance(__global const float *TIRAGES,
	      			  __global const int *nb_Simulation,
	      			  __global const int *nb_value_par_thread, 
	      			  __global float *esperance,
	      			  __global float *variance,  
	      			  __global const int *nb_THREAD,  
	      			  __global float *intervalConfiance,      	 
				  __global float *CARRE, 		  /*tableau de taille nombre processeur */
				  __global float *ESPERANCE)
{		  
  int i = get_global_id(0)*(*nb_value_par_thread);
  float tmp_esp;
  float tmp_carre;
  for(int t = 0; t < (*nb_value_par_thread); t++) {
    tmp_esp=tmp_esp+TIRAGES[i+t];
    tmp_carre=tmp_carre+TIRAGES[i+t]*TIRAGES[i+t];
  } 
  CARRE[get_global_id(0)]=tmp_carre;
  ESPERANCE[get_global_id(0)]=tmp_esp;


  barrier(CLK_LOCAL_MEM_FENCE);  
  if(get_global_id(0) == 0){
    float carre = 0.0;
    for(int j=0; j < (*nb_THREAD) ; j++ ){
      (*esperance)=(*esperance) + ESPERANCE[j]/ *nb_Simulation;
      carre=carre+CARRE[j]/(*nb_Simulation-1);
    }
    float denuminator = sqrt(1.0* (*nb_Simulation));
   *variance = carre -(*nb_Simulation)*(*esperance)*(*esperance);
   *intervalConfiance = 1.96* (*variance)/denuminator; 
  }
}
