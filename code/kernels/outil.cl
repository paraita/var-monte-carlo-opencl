__kernel void calcul_variance(__global const float *TIRAGES,
	      			  __global int *nb_Simulation,
	      			  __global int *nb_value_par_thread, 
	      			  __global float *esperance,
	      			  __global float *variance,  
	      			  __global int *nb_THREAD,  
	      			  __global float *intervalConfiance,      	 
				  __global float *CARRE, 		  /*tableau de taille nombre processeur */
				  __global float *ESPERANCE){		  /*tableau de taille nombre processeur */
  int i = get_global_id(0)*(*nb_value_par_thread); // offset
  float tmp_esp;
  float tmp_carre;
  for(int t = 0; t < (*nb_value_par_thread); t++) {
    tmp_esp=tmp_esp+TIRAGES[i+t];
    tmp_carre=tmp_carre+TIRAGES[i+t]*TIRAGES[i+t];
  }
  CARRE[get_global_id(0)]=tmp_carre;
  ESPERANCE[get_global_id(0)]=tmp_esp;

  barrier(CLK_LOCAL_MEM_FENCE); /*on attend que tout les Thread ont fini*/

  if(get_global_id(0)==0){
    float esperance = 0.0;
    float carre =0.0;
    for(int i=0; i < nb_THREAD ; i++ ){
       esperance+=ESPERANCE[i]/ *nb_Simulation;
       carre+=CARRE[i]/(*nb_Simulation-1);
    }

    *variance = carre - n*esperance*esperance;
    *intervalConfiance = 1.96*variance/sqrt(*nb_Simulation*1.0);
  }
}
