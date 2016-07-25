#include <iostream>
#include <time.h>
#include <vector>
#include <algorithm>
//#include <cstdio>

using namespace std;

int main(){
	cout << "Start\n";
	#define TotNunTrials 28
	#define BlockLength 7
	#define ParamNumber 5
	#define RepNumber 2
	#define CondNumber 3

	double TargetList[7]={-45, -30, -15, 0, 15, 30, 45};
	double InitHeadAngList[1]={0};
	double FinalHeadAngList[2]={-15, 15};
	double Gain[2]={1.0, 24.0/15.0};
	double feedback;
	char cond_names[3][4]={{"V-V"},{"V-K"},{"K-K"}};

	for (int cond=0; cond<CondNumber; cond++){
		for (int rep=0; rep<RepNumber; rep++){
			double Trials[TotNunTrials][ParamNumber]={0};
			srand(time(NULL));
			int trl=0;
			for (int ih=0; ih<1; ih++){
				for (int trg=0; trg<7; trg++){
					for (int fh=0; fh<2; fh++){
						for (int g=0; g<2; g++){
							feedback=0.0;
							
							int pippo=(rand()%10+1);
							cout << pippo;
							if ((Gain[g]==1.0) & (pippo>=5)){
								feedback=1.0;
							}
							Trials[trl][0]=InitHeadAngList[ih];
							Trials[trl][1]=TargetList[trg];
							Trials[trl][2]=FinalHeadAngList[fh];
							Trials[trl][3]=Gain[g];
							Trials[trl][4]=feedback;
							trl++;
						}
					}
				}
			}

			vector<int> rand_trial_order;
			for (int trl=0;trl<TotNunTrials;trl++) rand_trial_order.push_back(trl);

			srand(time(NULL));
			random_shuffle(rand_trial_order.begin(),rand_trial_order.end());

			FILE *pfile;
			for (int block=0;block<TotNunTrials/BlockLength;block++){// for now I suppose that the total number of trial il multiple of the block length
				char filename[30];
				sprintf(filename,"%s_Ser%d_Block%d.seq",cond_names[cond],rep+1,block+1);
				pfile=fopen(filename,"w");
				for (int trl=0;trl<BlockLength;trl++){
					for (int param=0;param<ParamNumber;param++){
						fprintf(pfile,"%2.1f;",Trials[rand_trial_order.at(trl+block*BlockLength)][param]);
					}
					fprintf(pfile,"\n");
				}
				fclose(pfile);
			}
		}
	}
	return 0;


}