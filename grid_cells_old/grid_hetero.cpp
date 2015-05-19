/*-------------------------------------------------------------------- */
/* This program demonstrates a network model of the grid cells in mEC. */
/* It is part of the model in the paper                                */
/*  The emergence of grid cells: Intelligent design or just adaptation?*/
/*  Kropff E, Treves A., Hippocampus. 2008;18(12):1256-69.             */
/*                                                                     */                
/* If you find any problems, please contanct: bailusi@sissa.it         */
/*                                                                     */
/*---------------------------------------------------------------------*/

// Squared weights sum up to 100.
// Environmentsize 250
// Mean activity decreased - NO
// Variable learning rate

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include "grid.h"

#define Steps 100

int main(){
  int i, j;
  bool control_mean;
  float sum1, sum2, factor;
  struct timeb time_start, time_curr;
  long int diff;
  FILE *W_evol, *W_evoltimes, *W_score, *W_average, *W_fraction, *W_aux;

  ftime(&time_start);

  // Random Sequence Initialization
  srand(1); 

  // Making dirs
  make_dirs();
  sprintf(filename,"%s%cEvolution.dat",foldername, 47);
  W_evol = fopen(filename,"w");
  sprintf(filename,"%s%cEvol_Times.dat",foldername, 47);
  W_evoltimes = fopen(filename,"w");
  sprintf(filename,"%s%cEvolGridScores.dat",foldername, 47);
  W_score = fopen(filename,"w");
  sprintf(filename,"%s%cEvolMeanGridScores.dat",foldername, 47);
  W_average = fopen(filename,"w");
  sprintf(filename,"%s%cEvolThresholdAndProportionGridCells.dat",foldername, 47);
  W_fraction = fopen(filename,"w");
  
  // Setting place cells in a lattice
  set_input();
  sprintf(filename,"%s%cGrid_PlaceUnits.dat",foldername, 47);
  W_aux=fopen(filename,"w");
  for(i=0;i<numhippunit;i++){    
    fprintf(W_aux,"%d %f %f\n", i+1, x_place_cell[i], y_place_cell[i]);
    } // end for i
  fclose(W_aux);

  // Initializating dynamics (input, adaptation variables, and output + weights from Hipp to EC + initial location)
  initialize();
  sprintf(filename,"%s%cConjunctiveProps.dat",foldername, 47);
  W_aux=fopen(filename,"w");
  for(i=0;i<numecunit;i++){
    fprintf(W_aux,"%d %f %f %f %f\n",i+1,(180./M_PI)*ConjCell_pref_HD[i],ConjCell_X[i],ConjCell_Y[i],b1[i]);
	}
  fclose(W_aux);
  sprintf(filename,"%s%cLat_weights.dat",foldername, 47);
  W_aux=fopen(filename,"w+");
  for(i=0;i<numecunit;i++){
    for(j=0;j<numecunit;j++) fprintf(W_aux,"%.3f ",weight_lateral[i][j]);
    fprintf(W_aux,"\n");
    }
  fclose(W_aux);

  // Evolution
  Nsteps_not_fixed = 0;
  for(idstep=0;idstep<plotstep*Steps;idstep++){
    if(idstep%10000==0) printf("Iteration %d\n",idstep);

    // Update Rat Position
    update_position();

    // Updating neurons according to current position
    control_mean = (idstep > 15);
    update_neurons(control_mean);

    // Storing history for some variables
    idhis = idstep%lenhis;
    speed_list[idhis] = speed; // nevertheless, speed is constant in this simulation
    x_list[idhis] = x;
    y_list[idhis] = y;
    theta_list[idhis] = theta; // movement direction
    mean_list[idhis] = mean_network_act;
    sparsity_list[idhis]= sparsity;
    
    if(idstep<100000){
	  sum1 = 0.;
      for(i=0;i<nconnhipp2ec;i++)
        sum1 += weight_hipp2ec[0][i]*act_hipp[i];
	  sum2 = 0.;
	  for(i=0;i<numecunit;i++)
	    sum2 += weight_lateral[0][i]*delayed_act_ec[i][0];
      factor = (float)idstep/(float)(plotstep*Steps/4.);
      if (factor > 1.) factor = 1.0;

      ftime(&time_curr);
      diff = (int) (1000.0 * (time_curr.time - time_start.time) + (time_curr.millitm - time_start.millitm));

	  fprintf(W_evol,"%d %d %ld %f %f %f %f %f %f %f %f %f %f\n",idstep,n_iter,diff,mean_network_act,sparsity,x,y,sum1,sum2,fdir(theta,ConjCell_pref_HD[0])*(sum1+rho*factor*sum2),active_ec[0],inactive_ec[0],act_ec[0]);
      fflush(W_evol);
	  }
      
    // Once output of the system is defined, learning in connections is applied
    update_weights();
    update_mean();

    // Update positional maps of activity of individual neurons
    // (according to a smoothing procedure, which takes into account ~100 historical values)
    update_maps();
      
    // Creates auto-correlograms, separated by plotstep steps 
    if(idstep%plotstep==0){
      ftime(&time_curr);
      diff = (int) (1000.0 * (time_curr.time - time_start.time) + (time_curr.millitm - time_start.millitm));
	  fprintf(W_evoltimes,"%d %ld ",idstep,diff);
	  
	  printf("Step %d out of %d: " , idstep, plotstep*Steps);
      compute_autocorrelation(auto_corre, map_pos);
      compute_gridness(auto_corre,grid_score,0);

      fprintf(W_score,"%d ",idstep);
      for(i=0;i<numecunit;i++)
	    fprintf(W_score,"%.3f ",grid_score[i]);
      fprintf(W_score,"\n");
      fflush(W_score);
	  
      compute_statistics(grid_score);
	  fprintf(W_average,"%d %.3f %.3f\n",idstep,mean,sqrt(var));      
      fflush(W_average);

      save_all();

      // Computing shuffled data
      compute_shuffling();
	  fprintf(W_fraction,"%d %f %f %f %f\n", idstep, percentil, frac, mean, sqrt(var));
      fflush(W_fraction);
      
      ftime(&time_curr);
      diff = (int) (1000.0 * (time_curr.time - time_start.time) + (time_curr.millitm - time_start.millitm));
	  fprintf(W_evoltimes,"%ld\n",diff);
      fflush(W_evoltimes);
	  } // end if idstep
     
    } // end for idstep

  printf("Not fixed %d out of %d\n",Nsteps_not_fixed,plotstep*Steps);

  fclose(W_evol);
  fclose(W_evoltimes);
  fclose(W_score);
  fclose(W_average);
  fclose(W_fraction);

  return(1);
}

// makes directories, basically one for each neuron in mec
void make_dirs(){
  int i;
  char command[500];

  sprintf(foldername,"Simulation-%d",1);
  sprintf(command,"mkdir %s", foldername);
  system(command);

  for(i=0;i<numecunit;i++){
    sprintf(command,"mkdir %s%ccell_%d", foldername, 47, i+1);
	system(command);
    }

  return;
}

//generates center coordinates of input maps that are like hippocampal place cells arranged evenly across the environment
void set_input(){
  int i, j, total, n;
  float r, spacing, radius, ang;

  total = 0;
 
  // Primer elemento a radio nulo
  i = 0; total++;
  radius = 0.;
  ang = 0.;
  x = radius*cos(ang);
  y = radius*sin(ang);
  r = map_perturb*(-.5+ uniform());
  x_place_cell[i] = x + environmentsize/2. + r;
  r = map_perturb*(-.5+ uniform());
  y_place_cell[i] = y + environmentsize/2. + r;
  
  spacing = 2.*M_PI*(environmentsize/2.) / (float)ext_sidehippgrid;
  
  for(i=1;i<=Nrings;i++){
    radius = (i/(float)Nrings)*(environmentsize/2.);
    n = (int) floor(2.*M_PI*radius/spacing + 0.5);
	ang = (2.*M_PI/(float)n) * uniform();
	for(j=0;j<n;j++){
	  ang += (2.*M_PI/(float)n);
      x = radius*cos(ang);
      y = radius*sin(ang);
      r = map_perturb*(-.5+ uniform());
      x_place_cell[total] = x + environmentsize/2. + r;
      r = map_perturb*(-.5+ uniform());
      y_place_cell[total] = y + environmentsize/2. + r;
	  total++;
	  }
	}

  // Una capa mas por afuera del circulo
  radius = ((Nrings+1)/(float)Nrings)*(environmentsize/2.);
  n = (int) floor(2.*M_PI*radius/spacing + 0.5);
  ang = (2.*M_PI/(float)n) * uniform();
  for(j=0;j<n;j++){
    ang += (2.*M_PI/(float)n);
    x = radius*cos(ang);
    y = radius*sin(ang);
    r = map_perturb*(-.5+ uniform());
    x_place_cell[total] = x + environmentsize/2. + r;
    r = map_perturb*(-.5+ uniform());
    y_place_cell[total] = y + environmentsize/2. + r;
    total++;
    }

  numhippunit = total;
  nconnhipp2ec = total;

  printf("Hippocampal units: %d\n",numhippunit);

  // Setting time constants in the network
  for(i=0;i<numecunit;i++){
    //b1[i] = 0.025 + (0.250-0.025)*i/(double)(numecunit-1);
    b1[i] = 0.100;
    b2[i] = b1[i]/3.;
	}

  return;
}

//initialize variables
void initialize(){
  int i,j,k,l;
  float sum_weight, angle, dist;

  speed = constspeed;
  threshold = 0.;

  for(i=0;i<numecunit;i++){
    act_ec[i] = 0.;       // entorhinal neurons state
    mean_act_ec[i] = 0.;
    active_ec[i] = 0.;    // adaptation variable
    inactive_ec[i] = 0.;  // adaptation variable
    input2ec[i] = 0.;     // activation of previous variables 
    for(k=0;k<delay;k++){
      delayed_act_ec[i][k] = 0.;
	  } // end for k
    for(k=0;k<numbinmap;k++){
      map_ang[i][k] = 0.;
      for(l=0;l<numbinmap;l++){
        map_pos[i][k][l] = 0.;
        for(j=0;j<numshuffled;j++) map_pos_shuffled[j][i][k][l] = 0.;
		} // end for l
	  } // end for k
	} // end for i

  for(j=0;j<numhippunit;j++){
    act_hipp[j] = 0.;     //hippocampal neurons state
    mean_act_hipp[j] = 0.;
    } // end for j
  
  for(i=0;i<numecunit;i++){
    sum_weight = 0.;
    for(k=0;k<nconnhipp2ec;k++){
	  weight_hipp2ec[i][k] = 0.5 + 1.0*uniform(); //random weights (from hc to ec) initialization
	  sum_weight +=  weight_hipp2ec[i][k]*weight_hipp2ec[i][k];
      } // end for k
      
    for(k=0;k<nconnhipp2ec;k++)//normalization
	  weight_hipp2ec[i][k] *= sqrt(1./sum_weight);
    
    } // end for i

  // Lateral connectivity - HD cell + Virtual associated cell
  for(i=0;i<numecunit;i++){
    ConjCell_pref_HD[i] = 2.*M_PI*uniform();
    do{
      ConjCell_X[i] = environmentsize * uniform();
      ConjCell_Y[i] = environmentsize * uniform();
      }
	while(isin(ConjCell_X[i],ConjCell_Y[i])==0);
	}

  // Set weights
  for(i=0;i<numecunit;i++){
    sum_weight = 0.;
    for(k=0;k<numecunit;k++){
      if(k!=i){

	    angle = atan2(ConjCell_Y[i]-ConjCell_Y[k], ConjCell_X[i]-ConjCell_X[k]);
		angle = modulus(angle,2.*M_PI);

        dist = sqrt( pow(ConjCell_X[i]-ConjCell_X[k]-expect_length*cos(angle),2) + pow(ConjCell_Y[i]-ConjCell_Y[k]-expect_length*sin(angle),2) );
  	    weight_lateral[i][k] = fdir(angle,ConjCell_pref_HD[k])*fdir(angle,ConjCell_pref_HD[i])*exp(-dist*dist/(2.*spreadlateral*spreadlateral)) - kappa;
        if (weight_lateral[i][k] < 0.) weight_lateral[i][k] = 0.;
		sum_weight +=  weight_lateral[i][k]*weight_lateral[i][k];
  	    }
  	  else
  	    weight_lateral[i][k] = 0.;
	  } // end for k

    if(sum_weight>0.){
	  for(k=0;k<numecunit;k++)
  	    weight_lateral[i][k] *= sqrt(1./sum_weight);
      }

	} // end for i
  
  // Initial position
  x = -1.;
  y = -1.;
  while(isin(x,y)==0){ // While the position is outside, keep doing. Until position is in the box
    x = environmentsize * uniform();
    y = environmentsize * uniform();
    }

  return;
}

//update the position with a step v. If the new position is out of the box, choose another one
void update_position(){
  float theta0, x0, y0, spread_turn0;
  float par, coef_a, coef_b, coef_c, xt, yt, alfa, theta0r;
  int i, count = 0;
	
  spread_turn0 = spreadturn;

  do{
    theta0 = theta + spread_turn0 * normal();
    x0 = x + speed * cos(theta0);
    y0 = y + speed * sin(theta0);
    
    if(isin(x0,y0)==0){
      coef_a = 1.;
	  coef_b = 2.0*cos(theta0)*(x-environmentsize/2.0) + 2.0*sin(theta0)*(y-environmentsize/2.0);
      coef_c = (x-environmentsize/2.0)*(x-environmentsize/2.0) + (y-environmentsize/2.0)*(y-environmentsize/2.0) - environmentsize*environmentsize/4.0;
	  par = (-coef_b + sqrt(coef_b*coef_b-4.0*coef_a*coef_c))/(2.0*coef_a);
	  	  
      xt = x + par * cos(theta0);
      yt = y + par * sin(theta0);

      alfa = atan2(-(xt-environmentsize/2.0),(yt-environmentsize/2.0));
      
	  // reflejamos
	  theta0r = 2.0*alfa - theta0;
	  theta0 = theta0r;
      x0 = xt + (speed-par) * cos(theta0);
      y0 = yt + (speed-par) * sin(theta0);
	  }

    //spread_turn0 *=1.1;
    //if(spread_turn0> 2.*M_PI) spread_turn0 = 2.*M_PI;
    
    //count++; 
    //if(count>50){
    //  printf("Stuck near a wall !!!!!!\n");
    //  count = 0;
    //  }

    } //end do
  while(isin(x0,y0)==0); // up to position is inside (at least, one updating)

  // Updating global variables
  theta = modulus(theta0,2.*M_PI);
  x = x0;
  y = y0;
  
  x_map = (int) floor(x*numbinmap/environmentsize);	// Pixeling the position (corresponding bin)
  y_map = (int) floor(y*numbinmap/environmentsize);
  theta_map = (int) floor(theta*numbinmap/(2.*M_PI));

  for(i=0;i<numshuffled;i++){
    do{
	  x_shuffled[i] = environmentsize*uniform();
      y_shuffled[i] = environmentsize*uniform();
      } //end do
    while(isin(x_shuffled[i],y_shuffled[i])==0); // up to position is inside (at least, one updating)
    
	x_map_shuffled[i] = (int) floor(x_shuffled[i]*numbinmap/environmentsize);
    y_map_shuffled[i] = (int) floor(y_shuffled[i]*numbinmap/environmentsize);
	}

  return;
}

//main part of the program, updates all mEC neurons by integrating feedforward activity
void update_neurons(bool control_mean){
  int i,j,k;
  float input, factor;

  for(j=0;j<numhippunit;j++){
    act_hipp[j]= fhc(x - x_place_cell[j],y - y_place_cell[j]);     //activation in hippocampus (time t)
    }

  for(i=0;i<numecunit;i++){
    // Updating input to EC (from Hippocampus - time t)
    input = 0.;
    for(k=0;k<nconnhipp2ec;k++)
      input += weight_hipp2ec[i][k]*act_hipp[k];

    input2ec[i] = input;

    // Updating input to EC (from lateral connectivity - time t)
	input = 0.;
	for(k=0;k<numecunit;k++)
	  input += weight_lateral[i][k]*delayed_act_ec[k][0];

    //factor = (float)idstep/(float)(plotstep*Steps/4.);
    //if (factor > 1.) factor = 1.0;
    factor = 1.0;
    input2ec[i] += rho*factor*input;
    input2ec[i] *= fdir(theta, ConjCell_pref_HD[i]);
    
    // Updating adaptation variables, including selection of most/less active cells (time t+1)
    active_ec[i] += b1[i]*(input2ec[i]-inactive_ec[i]-active_ec[i]);  //adaptation variable (activation)
    inactive_ec[i] += b2[i]*(input2ec[i]-inactive_ec[i]);             //adaptation variable (inactivation)
    
    if(i==0){
      min_active_ec = active_ec[0];  // EC neuron less active (initial arbitrary selection)
      max_active_ec = active_ec[0];  // EC neuron more active (initial arbitrary selection)
       }
    else{
      if(active_ec[i] < min_active_ec) min_active_ec = active_ec[i];
      if(active_ec[i] > max_active_ec) max_active_ec = active_ec[i];
      }

	} // end for i

  networkoutput(control_mean);

  return;
}

void networkoutput(bool control_mean){
  int i, j;
  
  if(!control_mean){
    ec_output();
    return; 
    } // get out without any control
  
  n_iter = 0;

  bool fixed = false;
  bool go_on = true;
  
  if(threshold > max_active_ec - 0.01) //choose a good initial threshold
    threshold = max_active_ec - sparsitylevel * (max_active_ec - min_active_ec);
  
  do{
    fix_sparsity();                   //fix sparsity (but maybe not, depending on "nummaxiter").
    fixed = is_fixed();

    if(!fixed &&  n_iter <= nummaxiter){
      comput_gain_gradient();        //fix mean by gradient descent
      adapt_gain();
      }

    go_on = ((!fixed) && (n_iter <= nummaxiter)); // to close the loop, but maybe not fixed!
    } // end do
  while(go_on);

  ec_output(); // includes the last update

  // Updates delayed activity
  for(i=0;i<numecunit;i++){
     for(j=0;j<delay-1;j++){
       delayed_act_ec[i][j] = delayed_act_ec[i][j+1];
	   }
     delayed_act_ec[i][delay-1] = act_ec[i];
	 }

  if(n_iter>nummaxiter) Nsteps_not_fixed++; //printf(" Step %d - Control of network activity not reached\n",idstep);

  return;
}

void fix_sparsity(){
  float cur_spar_error;

  bool fixed = find_threshold_range(max_active_ec);  // sums one iteration
  bool go_on = (!fixed);

  while(go_on){
    ec_output(); // With the new threshold selected in find_threshold_range: (lower+upper)/2
    n_iter++;

    cur_spar_error = fabs(sparsity - sparsitylevel);
    go_on =  ((cur_spar_error > tolerancemean * sparsitylevel)  && n_iter <= nummaxiter);

    if(go_on){  //new threshold
	  
      if(sparsity < sparsitylevel){  //threshold is too high, move to a lower value
        upper_threshold = threshold;
        threshold = (lower_threshold + threshold) / 2.0;
        }
      else{
        lower_threshold = threshold;
        threshold = (upper_threshold + threshold) / 2.0;
        } // end else

      } // end if go_on

    // A go condition can be reached by iterations, with a poor performance in cur_spar_error.
    } // end while 

  inc_threshold = upper_threshold - lower_threshold;
  inc_threshold = Max(inc_threshold, 0.005);
}

//find a threshold range to bound the sparsity
//return true if sparsity criteria is satisfied
bool find_threshold_range(float max_act){
  int direction;
  float cur_spar_error;
  
  ec_output();
  n_iter++;

  cur_spar_error = fabs(sparsity - sparsitylevel);
  if(cur_spar_error < tolerancemean * sparsitylevel)
    return true;

  if(sparsity < sparsitylevel){
    direction = -1;
    search_threshold(direction, upper_threshold, lower_threshold); //search for a lower threshold
    }
  else{
    direction = 1;
    search_threshold(direction, lower_threshold, upper_threshold); //search for a higher threshold
    }

  upper_threshold = Min(upper_threshold, max_act);

  threshold = (lower_threshold + upper_threshold) / 2.0;//save threshold for bisect search.
     
  return false; //need to do bisection further.
}

void ec_output(){
  int i;
  float act;

  mean_network_act = 0;
  mean_square_network_act = 0;

  for(i=0;i<numecunit;i++){
    act = active_ec[i] - threshold;
    
    if(act < 0.)
      act_ec[i]=0.;
    
    else
      act_ec[i]= firingconst * atan(gain * act);//state of ec unit. maximal rate is 1.

    mean_network_act += act_ec[i];
    mean_square_network_act += act_ec[i]*act_ec[i];
    } // end for

  mean_network_act = mean_network_act / (float)numecunit;  //average activity in EC: <r>
  if(mean_square_network_act > 1e-6)
    mean_square_network_act = mean_square_network_act / (float)numecunit;  //average dispersion in EC <r^2>
  else 
    mean_square_network_act = 1e-6;

  sparsity = mean_network_act * mean_network_act / mean_square_network_act;

  return;
}

//search for threshold range, return to root.
//When direction = 1, ascending search, and find [back_bound, ahead_bound].
//When direction = -1, descending search, and find [ahead_bound, back_bound]
void search_threshold(int direction, float &back_bound, float &ahead_bound){
  bool go_on = true;
  bool search_low = (direction == -1);

  inc_threshold = Max(inc_threshold, 0.005);  // inc_threshold is posteriorly updated according to upper/lower values; actually, this line doesn't care (it is already done before)
 
  back_bound = threshold; // current threshold is good as a back bound.
  
  while(go_on){
    threshold += direction * inc_threshold;

    ec_output();          // re-calculate network global state with this new threshold
    n_iter++;

    if((sparsity > sparsitylevel && search_low) || (sparsity < sparsitylevel && !search_low)){  //ahead bound found.
      ahead_bound = threshold; 
      go_on = false;
      }
    else{ //update back bound.
      back_bound = threshold;  
      inc_threshold *= 4;
      } 
    } // end while

  return;
}

bool is_fixed(){
  bool fixed = false;

  float cur_spar_error = fabs(sparsity - sparsitylevel);
  float cur_mean_error = fabs(mean_network_act - meanactivitylevel);

  fixed  = ((cur_spar_error < tolerancemean * sparsitylevel) && (cur_mean_error < tolerancemean * meanactivitylevel));
 
  return fixed;
}

void comput_gain_gradient(){
  int i;
  float dif, d_gain, denorm;
  
  d_mean_gain = 0.;

  for(i=0;i<numecunit;i++){
    dif = active_ec[i] - threshold;

    d_gain = 0.;
    if(dif > 0.){
      denorm = 1. / (1. + gain * gain *dif * dif);
	  d_gain = firingconst * dif * denorm;
	  }
      
    d_mean_gain += d_gain;
    } // end for
  
  d_mean_gain/= (float)numecunit;

  return;
}

void adapt_gain(){
  float f_mean, inc_gain, len, new_gain;

  f_mean = mean_network_act - meanactivitylevel;
  inc_gain = 0;
  
  if(fabs(d_mean_gain) > 1e-7){      // if mean gradient obtained before is significant
    inc_gain = f_mean / d_mean_gain; // increment in g
    len = fabs(inc_gain);
    if(len > 1000)
      inc_gain *= 1000.0 / len;       // limited to 1000 (+ or -)
    }
  else{  // mean gradient too small; not useful to update gain
    if(fabs(f_mean) > tolerancemean * meanactivitylevel){
      if(f_mean > 0) //mean too high, reduce gain
	    inc_gain = gain/(1.0+n_iter);
      else
	    inc_gain = -gain/(1.0+n_iter);
      }
    }

  new_gain = gain - inc_gain;

  if(new_gain < mingain || new_gain > maxgain){ //keep in safe range.
    new_gain = Max(new_gain, mingain);
    new_gain = Min(new_gain, maxgain);
    }

  gain = new_gain;

  return;
}

//learning weights
void update_weights(){
  int i,k;
  float sum_weight, lrweight;

  lrweight = lrweight1 + (lrweight2-lrweight1)*idstep/((float)plotstep*Steps/4.);
  if (lrweight < lrweight2) lrweight = lrweight2;

  for(i=0;i<numecunit;i++){
    sum_weight = 0.;
	
    for(k=0;k<nconnhipp2ec;k++){
      weight_hipp2ec[i][k] += lrweight * (act_ec[i]*act_hipp[k] - mean_act_ec[i] * mean_act_hipp[k]);
      
      if(weight_hipp2ec[i][k]<0.)
	    weight_hipp2ec[i][k]=0.;
	  else
	    sum_weight += weight_hipp2ec[i][k] * weight_hipp2ec[i][k]; 
	  } // end for k
      
    if(sum_weight > 1e-20){
	  for(k=0;k<nconnhipp2ec;k++)//normalization
	    weight_hipp2ec[i][k] *= sqrt(1./sum_weight);
      }
    else{
	  printf(" Weight diverge %e!\n", sum_weight);
	  for(k=0;k<nconnhipp2ec;k++)
	    weight_hipp2ec[i][k] = 1.0/sqrt((float)nconnhipp2ec);	  
      } // end else
    
    } // end for i

  return;
}

// mean activity in each neuron (averaging ~20 time steps)
void update_mean(){
  int i;

  for(i=0;i<numhippunit;i++)
    mean_act_hipp[i] += lrmean * (act_hipp[i] - mean_act_hipp[i]);
	  
  for(i=0;i<numecunit;i++)
    mean_act_ec[i] += lrmean * (act_ec[i] - mean_act_ec[i]);

  return;
}

//updates the map of each mec cell by averaging the last few times the rat passed through this place
void update_maps(){
  int i, j, pos_x, pos_y;
  
  for(i=0;i<numecunit;i++){
    map_pos[i][x_map][y_map] += lrpositionmap*(act_ec[i]-map_pos[i][x_map][y_map]);
    map_ang[i][theta_map] += lranglemap*(act_ec[i]-map_ang[i][theta_map]);
    for(j=0;j<numshuffled;j++){
      pos_x = x_map_shuffled[j];
      pos_y = y_map_shuffled[j];
	  map_pos_shuffled[j][i][pos_x][pos_y] += lrpositionmap*(act_ec[i]-map_pos_shuffled[j][i][pos_x][pos_y]);
      }
    }

  return;
}

//obtain auto-correlograms 
void compute_autocorrelation(float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float map[numecunit][numbinmap][numbinmap]){
  int sideY = numbinmap, sideX = numbinmap;
  int i, xOverlap, yOverlap, nOverlap, iXMin, iXMax, iX, iY;
  float aux, aux1, aux2, aux3, aux4, aux5, act1, act2;

  for(i=0;i<numecunit;i++)
    for(yOverlap = 1; yOverlap< sideY+1; yOverlap++)
      for(xOverlap =1; xOverlap < 2 * sideX; xOverlap++){
		nOverlap = 0;

		aux=0.;
		if(xOverlap > sideX){
          nOverlap = yOverlap *(2* sideX- xOverlap);
          iXMin = xOverlap - sideX;  // when xOverlap = sideX+1 :: all pixels except one are available, corresponds to tauX = (+/-) 1
          iXMax = sideX;             // when xOverlap = 2*sideX-1 :: just one pixel is available, corresponds to tauX = (+/-)(numbinmap-1) 
		  }
		else{
          nOverlap = xOverlap * yOverlap;
          iXMin = 0;         
          iXMax = xOverlap;  // when xOverlap = sideX :: all pixels are available, corresponds to tauX = 0
          }                  // when xOverlap = 1 :: just one pixel is available, corresponds to tauX = (+/-)(numbinmap-1)

		if(nOverlap > 6){
          aux1=0.; aux2=0.; aux3=0.; aux4=0.; aux5=0.;
          for(iY =0; iY < yOverlap; iY++)        // which pixels to span
            for(iX = iXMin; iX < iXMax; iX++){   // which pixels to span
			  act1 = map[i][iY][iX];
			  act2 = map[i][sideY-yOverlap+iY][sideX-xOverlap+iX];

			  aux1 += act1 * act2;
			  aux2 += act1;
			  aux3 += act2;
			  aux4 += act1 * act1;
			  aux5 += act2 * act2;
              } // end for iX

          // all sums have been calculated, for a given (tauX,tauY), whenever statistics is adequate (n>6)
          aux = (nOverlap*aux4 - aux2*aux2)*(nOverlap*aux5 - aux3*aux3);
          if(aux<1e-20){ // if variance is negligible across any two locations separated by (tauX,tauY) 
            aux = 0;
            }
          else
            aux = (nOverlap*aux1 - aux2*aux3)/sqrt(aux);

          } // end if(nOverlap > 6)

		ac[i][yOverlap-1][xOverlap-1] = aux;
		ac[i][2*sideY-yOverlap-1][2*sideX-xOverlap-1] = aux;
		} // end for xOverlap

  return;
}

void compute_gridness(float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float score[numecunit], int flag_print){
  int i;
  float UpperLimit, rmax_aux, aux;
  FILE *W1;

  assess_rmin(ac);
  //for(i=0;i<numecunit;i++) rmin[i] = 10.;

//  printf("  rmin\n");

  UpperLimit = ((numbinmap)*environmentsize/(float)numbinmap);

  for(i=0;i<numecunit;i++){
//    printf("    %d: %f -",i+1,rmin[i]);

    if(flag_print){
      sprintf(filename,"%s%ccell_%d%cScoreVsRadius-%d.dat",foldername, 47, i+1, 47, idstep);
      W1=fopen(filename,"w+");
      }

    rmax_aux = rmin[i] + environmentsize/10.; // Starts with an offset
    if(rmax_aux<Rmax_min) rmax_aux = Rmax_min;
	score[i] = -10.;
    rmax[i] = rmax_aux;
    while(rmax_aux<UpperLimit-environmentsize/10.){
      aux = assess_corr(i,ac,rmin[i],rmax_aux,0);
	  if(flag_print) fprintf(W1,"%f %f\n",rmax_aux,aux);
	  if(aux>score[i]){
	    score[i] = aux;
	    rmax[i] = rmax_aux;
	    }
      rmax_aux += environmentsize/(float)numbinmap;
  	  } // end while

//    printf(" %f\n",rmax[i]);

    if(flag_print) fclose(W1);

    // Once rmax[i] is determined, we print correspondind maps (if asked)
    aux = assess_corr(i,ac,rmin[i],rmax[i],flag_print);
    
    } // end for i

  return;
}

void assess_rmin(float ac[numecunit][2*numbinmap-1][2*numbinmap-1]){
  int i, j, k, n;
  float Dx, Dy;
  float UpperLimit, BinWidth;
  int Position, flag1, flag2, flag3;
  
  UpperLimit = sqrt(2)*((numbinmap)*environmentsize/(float)numbinmap);
  BinWidth = UpperLimit/(float)Nbins;
  
  for(i=0;i<numecunit;i++){ // loop over all EC cells

    for(j=0;j<(2*numbinmap-1);j++){   // loop over X in autocorrelogram
      for(k=0;k<(2*numbinmap-1);k++){ // loop over Y in autocorrelogram
	    Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
	    Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
	    Dist[j][k] = sqrt(pow(Dx,2)+pow(Dy,2));
        Corr[j][k] = ac[i][j][k];
		} // end for k 
      } // end for j

    for(j=0;j<Nbins;j++){
      BinData[i][j] = 0.;
      NData[j] = 0;
      }

    // Ordering data
    for(j=0;j<(2*numbinmap-1);j++){
      for(k=0;k<(2*numbinmap-1);k++){
        Position = (int) floor(Dist[j][k]/BinWidth);
        if(Position<Nbins)
		  BinData[i][Position] += Corr[j][k];
        else printf(" Error in determining Rmin\n");
		NData[Position]++;
		} // end for k 
      } // end for j

    rmin_aux[i] = 0;
    corr_aux[i] = 1.1;
    flag1 = 1;
    flag2 = 1;

    for(j=0;j<Nbins;j++){
      if(NData[j]!=0){
        BinData[i][j] /= (float)(NData[j]);
        if(flag1 && BinData[i][j]<corr_aux[i]){ // always decreasing up to minimum
          rmin_aux[i] = (j+0.5)*BinWidth;
          corr_aux[i] = BinData[i][j];
          
          rmin_extreme[i] = rmin_aux[i];        // stores the minimum value
		  if(flag2) rmin[i] = rmin_aux[i];      // by default; it is not, if 0.2 is reached before
		  if(flag2 && corr_aux[i]<0.2){
		    // interpolate
		    flag3 = 1;
		    n = 1;
			while(flag3 && (j-n) >= 0){
			  if(NData[j-n]==0){
			    n++;
				}
			  else flag3 = 0;
			  }
			// Interpolation
			rmin[i] = rmin_aux[i] + ((0.2-corr_aux[i])/(BinData[i][j-n]-corr_aux[i]))*((j-n+0.5)*BinWidth-rmin_aux[i]);
			flag2 = 0;
			} // end if (flag2)
		  } // end if (flag1)
		else
		  flag1 = 0; // leaves the previous loop (the first minimum was already caught) 
		}
  	  }

    } // end i
  
  return;
}

float assess_corr(int i, float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float r1, float r2, int print){
  int j, k, n;
  bool flag1;
  float Dx, Dy, Radius;
  float m0, m30, m60, m90, m120, m150;
  float v0, v30, v60, v90, v120, v150;
  float cross30, cross60, cross90, cross120, cross150;
  float corr30, corr60, corr90, corr120, corr150, corr_even, corr_odd;
  float aux1, aux2, corr;
  FILE *W0,*W30,*W60,*W90,*W120,*W150;
  
  if(print){
    sprintf(filename,"%s%ccell_%d%cOriginalMap-%d.dat",foldername, 47, i+1, 47, idstep);
    W0=fopen(filename,"w+");
    sprintf(filename,"%s%ccell_%d%cRotatedMap_30degrees-%d.dat",foldername, 47, i+1, 47, idstep);
    W30=fopen(filename,"w+");
    sprintf(filename,"%s%ccell_%d%cRotatedMap_60degrees-%d.dat",foldername, 47, i+1, 47, idstep);
    W60=fopen(filename,"w+");
    sprintf(filename,"%s%ccell_%d%cRotatedMap_90degrees-%d.dat",foldername, 47, i+1, 47, idstep);
    W90=fopen(filename,"w+");
    sprintf(filename,"%s%ccell_%d%cRotatedMap_120degrees-%d.dat",foldername, 47, i+1, 47, idstep);
    W120=fopen(filename,"w+");
    sprintf(filename,"%s%ccell_%d%cRotatedMap_150degrees-%d.dat",foldername, 47, i+1, 47, idstep);
    W150=fopen(filename,"w+");
	}


  n = 0;
  m0 = 0.; v0 =0.;
  m30 = 0.; v30 = 0.; cross30 = 0.; corr30 = 0.;
  m60 = 0.; v60 = 0.; cross60 = 0.; corr60 = 0.;
  m90 = 0.; v90 = 0.; cross90 = 0.; corr90 = 0.;
  m120 = 0.; v120 = 0.; cross120 = 0.; corr120 = 0.;
  m150 = 0.; v150 = 0.; cross150 = 0.; corr150 = 0.;
  
  rotate(i,ac,ac_rot30,30.*M_PI/180.0,r1,r2);
  rotate(i,ac,ac_rot60,60.*M_PI/180.0,r1,r2);
  rotate(i,ac,ac_rot90,90.*M_PI/180.0,r1,r2);
  rotate(i,ac,ac_rot120,120.*M_PI/180.0,r1,r2);
  rotate(i,ac,ac_rot150,150.*M_PI/180.0,r1,r2);

  for(j=0;j<2*numbinmap-1;j++){
	for(k=0;k<2*numbinmap-1;k++){
      // Truncated map within the anulus
      Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
      Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
      Radius = sqrt(pow(Dx,2)+pow(Dy,2));
      // Anulus condition
	  flag1 = (Radius>=r1 && Radius<r2);
	  
      if(flag1){
		m0 += ac[i][j][k];
		m30 += ac_rot30[j][k];
		m60 += ac_rot60[j][k];
		m90 += ac_rot90[j][k];
		m120 += ac_rot120[j][k];
		m150 += ac_rot150[j][k];
		n++;

        if(print){
          fprintf(W0,"%.3f ", ac[i][j][k]);
          fprintf(W30,"%.3f ", ac_rot30[j][k]);
          fprintf(W60,"%.3f ", ac_rot60[j][k]);
          fprintf(W90,"%.3f ", ac_rot90[j][k]);
          fprintf(W120,"%.3f ", ac_rot120[j][k]);
          fprintf(W150,"%.3f ", ac_rot150[j][k]);          
		  } // end if print
		} // end if
	  
	  else{
        if(print){
  	      fprintf(W0,"%.3f ", -10.);
	      fprintf(W30,"%.3f ", -10.);
	      fprintf(W60,"%.3f ", -10.);
	      fprintf(W90,"%.3f ", -10.);
	      fprintf(W120,"%.3f ", -10.);
	      fprintf(W150,"%.3f ", -10.);
		  }	// end if print
		} // end else
	  }
    
	if(print){
      fprintf(W0,"\n ");
      fprintf(W30,"\n ");
      fprintf(W60,"\n ");
      fprintf(W90,"\n ");
      fprintf(W120,"\n ");
      fprintf(W150,"\n ");
	  }
	
	}

  m0 /= (float)n; 
  m30 /= (float)n; 
  m60 /= (float)n; 
  m90 /= (float)n; 
  m120 /= (float)n; 
  m150 /= (float)n; 

  for(j=0;j<2*numbinmap-1;j++){
	for(k=0;k<2*numbinmap-1;k++){
      // Truncated map within the anulus
      Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
      Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
      Radius = sqrt(pow(Dx,2)+pow(Dy,2));
      // Anulus condition
	  flag1 = (Radius>=r1 && Radius<r2);
	  
      if(flag1){
		v0 += pow(ac[i][j][k]-m0,2);
		v30 += pow(ac_rot30[j][k]-m30,2);
		v60 += pow(ac_rot60[j][k]-m60,2);
		v90 += pow(ac_rot90[j][k]-m90,2);
		v120 += pow(ac_rot120[j][k]-m120,2);
		v150 += pow(ac_rot150[j][k]-m150,2);

        cross30 += (ac[i][j][k]-m0)*(ac_rot30[j][k]-m30);
        cross60 += (ac[i][j][k]-m0)*(ac_rot60[j][k]-m60);
        cross90 += (ac[i][j][k]-m0)*(ac_rot90[j][k]-m90);
        cross120 += (ac[i][j][k]-m0)*(ac_rot120[j][k]-m120);
        cross150 += (ac[i][j][k]-m0)*(ac_rot150[j][k]-m150);
		} // end if
	  }
    }

  corr30 = cross30/sqrt(v0*v30);
  corr60 = cross60/sqrt(v0*v60);
  corr90 = cross90/sqrt(v0*v90);
  corr120 = cross120/sqrt(v0*v120);
  corr150 = cross150/sqrt(v0*v150);

  aux1 = 0.; aux2 = 0.;
  if(corr60<=corr120) aux1 = corr60;
  else aux1 = corr120;
  if(corr30>=corr90 && corr30>=corr150) aux2 = corr30;
  if(corr90>=corr30 && corr90>=corr150) aux2 = corr90;
  if(corr150>=corr30 && corr150>=corr90) aux2 = corr150;
//  corr = aux1 - aux2;
  corr_even = 0.5*(corr60+corr120);
  corr_odd = (1./3.)*(corr30+corr90+corr150);
  corr = corr_even - corr_odd;

  if(print){
    fclose(W0);
    fclose(W30);
    fclose(W60);
    fclose(W90);
    fclose(W120);
    fclose(W150);
    }
  
  return(corr);
}

void rotate(int i, float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float ac_rot[2*numbinmap-1][2*numbinmap-1], float alpha, float r1, float r2){
  bool flag1;
  int j, k, j1, j2, k1, k2;
  float Dx, Dy, Radius, Dx_, Dy_, Dx_rot, Dy_rot, Q11, Q12, Q21, Q22;

  for(j=0;j<2*numbinmap-1;j++){
	for(k=0;k<2*numbinmap-1;k++){
      // Truncated map within the anulus
      Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
      Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
      Radius = sqrt(pow(Dx,2)+pow(Dy,2));

      // Anulus condition
	  flag1 = (Radius>=r1 && Radius<r2);

      if(flag1){
  	    // Rotated coordinates (with _ )
	    Dx_ = ((j+1) - numbinmap)*cos(alpha) + ((k+1) - numbinmap)*sin(alpha);
	    Dy_ = -((j+1) - numbinmap)*sin(alpha) + ((k+1) - numbinmap)*cos(alpha);
	    Dx_rot = floor(Dx_);
        Dy_rot = floor(Dy_);

        // Bilinear interpolation
        j1 = (int)Dx_rot + (numbinmap-1);
	    j2 = (int)Dx_rot + (numbinmap-1) + 1;
        k1 = (int)Dy_rot + (numbinmap-1);
	    k2 = (int)Dy_rot + (numbinmap-1) + 1;
      
        Q11 = ac[i][k1][j1];
        Q12 = ac[i][k2][j1];
        Q21 = ac[i][k1][j2];
        Q22 = ac[i][k2][j2];
	  
  	    ac_rot[k][j] = (1./(1.)*(1.)) * ( Q11*(Dx_rot+1-Dx_)*(Dy_rot+1-Dy_) + Q21*(Dx_-Dx_rot)*(Dy_rot+1-Dy_) + Q12*(Dx_rot+1-Dx_)*(Dy_-Dy_rot) + Q22*(Dx_-Dx_rot)*(Dy_-Dy_rot) );
        } // end if
	  }
    }

  return;
}

void compute_statistics(float score[numecunit]){
  int i, Position;
  float BinWidth;
 
  mean = 0.;
  for(i=0;i<numecunit;i++)
    mean += score[i];
  mean /= (float)numecunit;
      
  var = 0.;
  for(i=0;i<numecunit;i++)
    var += pow(score[i]-mean,2);
  var /= (float)(numecunit-1);

  for(i=0;i<Nbins_histo;i++){
    Histo[i] = 0.;
    }
    
  BinWidth = (CorrSup-CorrInf)/(float)Nbins_histo;
  for(i=0;i<numecunit;i++){
    Position = (int) floor((score[i]-CorrInf)/BinWidth);
    Histo[Position]++;
	}
  for(i=0;i<Nbins_histo;i++){
    Histo[i] /= (numecunit*BinWidth);
    }

  return;
}

//saves maps, autocorrelations and more
void save_all(){
  int i,j,k,l;
  float BinWidth;
  FILE *disp;
  
  for(i=0;i<numecunit;i++){
    sprintf(filename,"%s%ccell_%d%cweight-%d.dat",foldername, 47, i+1, 47, idstep);
    disp=fopen(filename,"w+");
    for(k=0;k<numhippunit;k++){
      fprintf(disp,"%d %.3f\n",k+1,weight_hipp2ec[i][k]);
      }
    fclose(disp);
		
    sprintf(filename,"%s%ccell_%d%cmap-%d.dat",foldername, 47, i+1, 47, idstep);
    disp=fopen(filename,"w+");
    for(l=0;l<numbinmap;l++){
      for(k=0;k<numbinmap;k++) fprintf(disp,"%.3f ",map_pos[i][k][l]); 
      fprintf(disp,"\n ");
      }
    fclose(disp);

    sprintf(filename,"%s%ccell_%d%cautoc-%d.dat",foldername, 47, i+1, 47, idstep);
    disp=fopen(filename,"w+");
    for(l=0;l<2*numbinmap-1;l++){
      for(k=0;k<2*numbinmap-1;k++) fprintf(disp,"%.3f ",auto_corre[i][l][k]);
      fprintf(disp,"\n ");
      }
    fclose(disp);

    sprintf(filename,"%s%ccell_%d%cmapang-%d.dat",foldername, 47, i+1, 47, idstep);
    disp=fopen(filename,"w+");
    for(k=0;k<numbinmap;k++) fprintf(disp,"%.3f ",map_ang[i][k]);
    fprintf(disp,"\n ");
    fclose(disp);
    } // end for

  BinWidth = (CorrSup-CorrInf)/(float)Nbins_histo;

  sprintf(filename,"%s%cGridScore_Histogram-%d.dat",foldername, 47,idstep);
  disp=fopen(filename,"w+");
  for(l=0;l<Nbins_histo;l++) fprintf(disp,"%f %.3f\n",CorrInf+(l+0.5)*BinWidth,Histo[l]);
  fclose(disp);

  sprintf(filename,"%s%cpath-x.dat",foldername, 47);
  disp=fopen(filename,"w+");
  for(l=0;l<lenhis;l++) fprintf(disp,"%d %.3f\n",l,x_list[l]);
  fclose(disp);

  sprintf(filename,"%s%cpath-y.dat",foldername, 47);
  disp=fopen(filename,"w+");
  for(l=0;l<lenhis;l++) fprintf(disp,"%d %.3f\n",l,y_list[l]);
  fclose(disp); 

  sprintf(filename,"%s%cpath-theta.dat",foldername, 47);
  disp=fopen(filename,"w+");
  for(l=0;l<lenhis;l++) fprintf(disp,"%d %.3f\n",l,theta_list[l]);
  fclose(disp);

  return;
}

void compute_shuffling(){
  int i, j, k, flag, Ngrid;
  float sum, BinWidth, x1, x2, s1, s2;
  int j_max1, k_max1, j_max2, k_max2, j_max3, k_max3;
  float Dx, Dy, Radius, max, Distance1, Distance2;
  FILE *disp;
    
  printf("Shuffling ");
  for(i=0;i<numshuffled;i++){
    printf("%d " ,i+1);
//	compute_autocorrelation(auto_corre_shuffled, map_pos_shuffled[i]);
//    compute_gridness(auto_corre_shuffled,grid_score_shuffled[i],0);
//    compute_statistics(grid_score_shuffled[i]);
    }
  printf("\n");

  // compute distribution of grid scores on overall shuffling data
//  compute_statistics_shuffled(grid_score_shuffled);

  // Computing 95% percentil - threshold for definition of grid cell
//  BinWidth = (CorrSup-CorrInf)/(float)Nbins_histo;
//  sum = 0.;
//  flag = 1;
//  for(i=0;i<Nbins_histo;i++){
//    sum += Histo[i]*BinWidth;
//    if(flag && sum > 0.95){
//      x1 = CorrInf+(i-0.5)*BinWidth;
//      x2 = CorrInf+(i+0.5)*BinWidth;
//      s1 = sum - Histo[i]*BinWidth;
//      s2 = sum;
//      percentil = x1 + ((x2-x1)/(s2-s1))*(0.95-s1);
//      flag = 0;
//      }
//    }

  percentil = 0.;

  // fraction and properties of grid cells (those with grid score above 95%-limit in the shuffled distribution)
  frac = 0.0;
  mean = 0.0;
  Ngrid = 0;
  for(i=0;i<numecunit;i++){
    if(grid_score[i]>percentil){
      frac += 1.0; 
      mean += grid_score[i];
      Ngrid++;
      }
    }
  frac /= (float)numecunit;
  mean /= (float)Ngrid;
  var = 0.0;
  for(i=0;i<numecunit;i++){
    if(grid_score[i]>percentil){
      var += pow(grid_score[i]-mean,2);
      }
    }
  var /= (float)(Ngrid-1);
	  
  // Printing shuffled distribution
  //sprintf(filename,"%s%cGridScoreShuffled_Histogram-%d.dat",foldername, 47, idstep);
  //disp=fopen(filename,"w");
  //sum = 0.;
  //for(i=0;i<Nbins_histo;i++){
  //  sum += Histo[i]*BinWidth;
  //  fprintf(disp,"%f %.3f %.3f\n",CorrInf+(i+0.5)*BinWidth,Histo[i],sum);
  //  }
  //fclose(disp);

  // For grid cells (those with grid score above 95%), we compute peaks of activity in the correlogram
/*  sprintf(filename,"%s%cGridProperties-%d.dat",foldername, 47, idstep);
  disp=fopen(filename,"w");
  compute_gridness(auto_corre,grid_score,0); // to recover values of rmin, etc
  for(i=0;i<numecunit;i++){
	if(grid_score[i]>percentil){

      if(rmin_extreme[i] < rmax[i]){

        // First peak
        max = -1.0;
        j_max1 = -1;
        k_max1 = -1;

        for(j=0;j<2*numbinmap-1;j++){
	      for(k=numbinmap-1;k<2*numbinmap-1;k++){ // just upper half of the plane
            // Truncated map within the anulus
            Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
            Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
            Radius = sqrt(pow(Dx,2)+pow(Dy,2));
            // Anulus condition (but starting in minimum by extreme condition)
	        flag = (Radius>=rmin_extreme[i] && Radius<rmax[i]);

            if(flag){  // inside meaningful part of the correlogram (region where grid score was calculated)
              if(auto_corre[i][k][j] > max){
                max = auto_corre[i][k][j];
			    j_max1 = j;
			    k_max1 = k;
			    } // end if max
              
			  } // end if flag

            } // end for k
          } // end for j

        Dx = ((j_max1+1) - numbinmap)*environmentsize/(float)numbinmap;
        Dy = ((k_max1+1) - numbinmap)*environmentsize/(float)numbinmap;
	    fprintf(disp,"%d %f %f %f %f %f %f ",i+1,rmin[i],rmin_extreme[i],rmax[i],Dx,Dy,max);

        // Second peak
        max = -1.0;
        j_max2 = -1;
        k_max2 = -1;

        for(j=0;j<2*numbinmap-1;j++){
	      for(k=numbinmap-1;k<2*numbinmap-1;k++){ // just upper half of the plane
            // Truncated map within the anulus
            Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
            Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
            Radius = sqrt(pow(Dx,2)+pow(Dy,2));
            // Anulus condition (but starting in minimum by extreme condition)
	        flag = (Radius>=rmin_extreme[i] && Radius<rmax[i]);

            // Distance to first detected peak
			Distance1 = sqrt( pow((j-j_max1)*environmentsize/(float)numbinmap,2) + pow((k-k_max1)*environmentsize/(float)numbinmap,2) );
            if(flag && Distance1 > r_avoid){  // inside meaningful part of the correlogram && far away from first peak
              if(auto_corre[i][k][j] > max){
                max = auto_corre[i][k][j];
			    j_max2 = j;
			    k_max2 = k;
			    } // end if max
              
			  } // end if flag

            } // end for k
          } // end for j

        Dx = ((j_max2+1) - numbinmap)*environmentsize/(float)numbinmap;
        Dy = ((k_max2+1) - numbinmap)*environmentsize/(float)numbinmap;
	    fprintf(disp,"%f %f %f ",Dx,Dy,max);

        // Third peak
        max = -1.0;
        j_max3 = -1;
        k_max3 = -1;

        for(j=0;j<2*numbinmap-1;j++){
	      for(k=numbinmap-1;k<2*numbinmap-1;k++){ // just upper half of the plane
            // Truncated map within the anulus
            Dx = ((j+1) - numbinmap)*environmentsize/(float)numbinmap;
            Dy = ((k+1) - numbinmap)*environmentsize/(float)numbinmap;
            Radius = sqrt(pow(Dx,2)+pow(Dy,2));
            // Anulus condition (but starting in minimum by extreme condition)
	        flag = (Radius>=rmin_extreme[i] && Radius<rmax[i]);

            // Distance to first detected peak
			Distance1 = sqrt( pow((j-j_max1)*environmentsize/(float)numbinmap,2) + pow((k-k_max1)*environmentsize/(float)numbinmap,2) );
            // Distance to second detected peak
			Distance2 = sqrt( pow((j-j_max2)*environmentsize/(float)numbinmap,2) + pow((k-k_max2)*environmentsize/(float)numbinmap,2) );
            if(flag && Distance1 > r_avoid && Distance2 > r_avoid){  // inside meaningful part of the correlogram && far away from previous peaks
              if(auto_corre[i][k][j] > max){
                max = auto_corre[i][k][j];
			    j_max3 = j;
			    k_max3 = k;
			    } // end if max
              
			  } // end if flag

            } // end for k
          } // end for j

        Dx = ((j_max3+1) - numbinmap)*environmentsize/(float)numbinmap;
        Dy = ((k_max3+1) - numbinmap)*environmentsize/(float)numbinmap;
	    fprintf(disp,"%f %f %f\n",Dx,Dy,max);

        } // end if (radii relationship)

      else{
	    fprintf(disp,"%d %f %f %f %f %f %f %f %f %f %f %f %f\n",i+1,rmin[i],rmin_extreme[i],rmax[i],0.0,0.0,-1.0,0.0,0.0,-1.0,0.0,0.0,-1.0);  
        }

      } // end if (grid cell)
    }
  fclose(disp);
*/
  return;
}

void compute_statistics_shuffled(float score[numshuffled][numecunit]){
  int i, j, Position;
  float BinWidth;
 
  for(i=0;i<Nbins_histo;i++){
    Histo[i] = 0.;
    }
    
  BinWidth = (CorrSup-CorrInf)/(float)Nbins_histo;
  for(i=0;i<numshuffled;i++){
    for(j=0;j<numecunit;j++){
      Position = (int) floor((score[i][j]-CorrInf)/BinWidth);
      Histo[Position]++;
	  }
	}
  for(i=0;i<Nbins_histo;i++){
    Histo[i] /= (numshuffled*numecunit*BinWidth);
    }

  return;
}

//activation of a simple place cell like input neuron
float fhc(float x0, float y0){
  return exp(-(x0*x0+y0*y0)/(2.*spreadplacefield*spreadplacefield));
}

//activation of a simple place cell like input neuron
float fdir(float angle, float pref_angle){
  return ( c0 + (1.-c0) * exp( nu*( cos(pref_angle-angle) - 1. ) ) );
}

//Is this position inside or outside the box?
int isin(float x0, float y0){
  float radius;

  // circle of diameter environmentsize
  radius = sqrt(pow(x0-environmentsize/2.,2) + pow(y0-environmentsize/2.,2));  
  
  if(radius<=environmentsize/2.0) return 1;
  else return 0;
}

// the remainder (modulus b), even for negative values.
float modulus(float a, float b){
  while(a <0.) a += b;
  return fmod(a,b);
}
// normal distribution
float normal(){
  return sqrt(-2.*log((rand()+1.)/((float)RAND_MAX+1.)))*sin(2.*M_PI*rand()/((float)RAND_MAX+1.));
}

//uniform distribution in [0 1).
float uniform(){
  return ((float)rand())/((float)RAND_MAX + 1.0);
}
