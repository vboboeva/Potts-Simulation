#define	environmentsize	  200.  //size of square room
#define	constspeed   0.4	    //speed of the rat
#define spreadturn    0.2       //angular head turn dispersion
#define	numbinmap      50       //pixels per dimension in a map
#define map_perturb     0.      //randomness in place fields.

#define	lrpositionmap	0.01	//speed of map update
#define lranglemap     0.1*lrpositionmap
#define plotstep   250000
#define lenhis    250000        //remember past steps.

#define	numecunit	100         //number of ec units
#define Nrings       30
#define ext_sidehippgrid    190
#define	numhippunit_aux	    Nrings*ext_sidehippgrid //number of place units
#define nconnhipp2ec_aux    numhippunit_aux
#define	spreadplacefield  5.0       //place field standard deviation
//#define	b1	0.100  		        //adaptation parameters
//#define	b2	b1/3.
#define b3 0.01
#define b4 0.1
#define lrmean    0.05          //averaging factor for firing rate (takes ~1000 steps).
#define	lrweight1	 0.005      //speed of weight learning
#define	lrweight2	 0.001      //speed of weight learning
#define sparsitylevel    0.3    //desired sparsity
#define meanactivitylevel 0.1   //desired mean
#define mingain          0.01
#define maxgain          50000.0
#define nummaxiter       1000   //max step in sparsity and mean control
#define tolerancemean    0.1    //relative error in spr mean control
#define firingconst      2.0/M_PI

#define c0 0.2
#define nu 0.5
#define spreadlateral  15.       //lateral connectivity standard deviation
#define kappa 0.05
#define expect_length 10.
#define delay 25
#define rho 0.2

#define Nbins 50
#define Nbins_histo 80
#define CorrSup 2.0
#define CorrInf -2.0
#define r_avoid 20.0             // to prevent close peaks in autocorrelogram
#define Rmax_min 45.0

#define numshuffled 5

#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Min(a, b) ((a) < (b) ? (a) : (b))

// Functions
void initialize();
float fhc(float, float);                   // place field
float fdir(float angle, float pref_angle); // head direction selectivity
void  update_position();
void update_neurons(bool control_mean);
void networkoutput(bool control_mean);
void ec_output();

void fix_sparsity();
bool find_threshold_range(float max_act);
void search_threshold(int direction, float &back_bound, float &ahead_bound);
bool is_fixed();
void  comput_gain_gradient();
void adapt_gain();

void update_maps();
void update_weights();
void update_mean();
void set_input();
void compute_autocorrelation(float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float map[numecunit][numbinmap][numbinmap]);
void compute_gridness(float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float score[numecunit], int flag_print);
void assess_rmin(float ac[numecunit][2*numbinmap-1][2*numbinmap-1]);
float assess_corr(int i, float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float r1, float r2, int print);
void rotate(int i, float ac[numecunit][2*numbinmap-1][2*numbinmap-1], float ac_rot[2*numbinmap-1][2*numbinmap-1], float alpha, float r1, float r2);
void compute_statistics(float score[numecunit]);
void compute_shuffling();
void compute_statistics_shuffled(float score[numshuffled][numecunit]);

int  isin(float, float); //is this position inside the box?
void make_dirs();
void save_all();
float normal();  //random normal generator
float uniform();
float modulus(float, float);

// Global variables
float x_place_cell[numhippunit_aux],y_place_cell[numhippunit_aux];	//centers of input distributions for HC (space)
float weight_hipp2ec[numecunit][nconnhipp2ec_aux];              //weights from hippocampus to entorhinal cortex
float act_ec[numecunit], act_hipp[numhippunit_aux];             //states of entorhinal and hippocampal neurons
float active_ec[numecunit],inactive_ec[numecunit],input2ec[numecunit];   	//adaptation variables for MEC
float mean_act_ec[numecunit],mean_act_hipp[numhippunit_aux];	//temporal mean of EC and HC
float mean_network_act, mean_square_network_act, sparsity;	//mean, square mean, sparsity EC activity
float threshold = 0, gain = 2;				                //threshold, gain of the trnasfer function.
float max_active_ec, min_active_ec;                             //range of active_ec.
int n_iter;
float inc_threshold = 0.005;
float upper_threshold = 1, lower_threshold = -1;      //threshold bound.
float d_mean_gain;                 //mean gradient of activity w.r.t. gain.

float b1[numecunit], b2[numecunit];

float map_pos[numecunit][numbinmap][numbinmap];             //spatial map of EC cells
float map_pos_shuffled[numshuffled][numecunit][numbinmap][numbinmap];             //shuffled spatial map of EC cells
float map_ang[numecunit][numbinmap];                        //angular map of EC cells
float auto_corre[numecunit][2*numbinmap-1][2*numbinmap-1];  //autocorrelatoin
float auto_corre_shuffled[numecunit][2*numbinmap-1][2*numbinmap-1];  //autocorrelatoin
float ac_rot60[2*numbinmap-1][2*numbinmap-1];
float ac_rot120[2*numbinmap-1][2*numbinmap-1];
float ac_rot30[2*numbinmap-1][2*numbinmap-1];
float ac_rot90[2*numbinmap-1][2*numbinmap-1];
float ac_rot150[2*numbinmap-1][2*numbinmap-1];

float theta, x, y;			        //head direction and position
float x_shuffled[numshuffled], y_shuffled[numshuffled];
float speed; 
int  x_map, y_map, theta_map;       //variables to construct maps
int x_map_shuffled[numshuffled], y_map_shuffled[numshuffled];

float  mean_list[lenhis];           //average activity
float  sparsity_list[lenhis];       //sparsity
float  x_list[lenhis], y_list[lenhis], theta_list[lenhis], speed_list[lenhis];
char foldername[500], filename[500];
int idstep;
int idhis;

int Nsteps_not_fixed;
float ConjCell_pref_HD[numecunit], ConjCell_X[numecunit], ConjCell_Y[numecunit];
float weight_lateral[numecunit][numecunit];
float delayed_act_ec[numecunit][delay];

float rmin[numecunit], rmin_extreme[numecunit], rmax[numecunit], grid_score[numecunit], grid_score_shuffled[numshuffled][numecunit];
float Dist[2*numbinmap-1][2*numbinmap-1], Corr[2*numbinmap-1][2*numbinmap-1], rmin_aux[numecunit], corr_aux[numecunit];
float BinData[numecunit][Nbins];
int NData[Nbins];

int numhippunit, nconnhipp2ec;

float mean, var, Histo[Nbins_histo], percentil, frac;
