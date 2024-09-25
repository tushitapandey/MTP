#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <random>
#include <cstdlib> 
using namespace std;
int numberOfPeople=10;
// Associate each man or woman ranker with an index
void generateNamesAndPreferences(bool print);
vector<map<string, string>>  GaleShapley();
vector<string> men;
vector<string> women;
map<string, vector<string>> menRanking;
map<string, vector<string>> womenRanking;
vector<string> initialRanking;
vector<string> temporaryRanking;
bool print = true;

int accross_trial_men_with_incentive=0;
int accross_trial_men_with_best_partner=0;

void Trial() {
    generateNamesAndPreferences(print);
    vector<map<string, string> > original_matches = GaleShapley();
    vector<map<string, string> > temporary_matches;
    int men_with_incentive=0,men_with_best_partner=0;
    for(string chosenMan: men){
        
        if(original_matches[0][chosenMan] == menRanking[chosenMan][0]){
            men_with_best_partner++;
            continue;
        }
        if(print)
            cout<<"Man "<<chosenMan<<" "<<original_matches[0][chosenMan]<<"\n Possible Accomplices: \n";
            
        int possible_accomplices=0;
        bool found_better=false;
        for (string& chosenWoman : menRanking[chosenMan]) {
            // if(found_better)
            //     break;
            // if(print)
            //     cout<<" for woman "<<chosenWoman<<" :\n";
            vector<string> initialRanking = womenRanking[chosenWoman];
            int swapRanking = find(initialRanking.begin(), 
                initialRanking.end(), original_matches[1][chosenWoman]) - initialRanking.begin() +1;
            vector<string> temporaryRanking = initialRanking;
            if(swapRanking==initialRanking.size())
                continue;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            
            womenRanking[chosenWoman]= temporaryRanking;
            // if(print){
            //     cout<<"ranking of woman is \n";
            //     for(auto c: womenRanking[chosenWoman]){
            //                 cout<<c<<" ";
            //             }
            //     cout<<"\n";

            // }
            
            temporary_matches = GaleShapley();
            int man_difference = find(menRanking[chosenMan].begin(),
                                            menRanking[chosenMan].end(), original_matches[0][chosenMan])
                                            - find(menRanking[chosenMan].begin(), 
                                            menRanking[chosenMan].end(), temporary_matches[0][chosenMan]);
            int woman_difference = find(womenRanking[chosenMan].begin(), 
                                            womenRanking[chosenMan].end(), temporary_matches[1][chosenWoman]) 
                                - find(womenRanking[chosenWoman].begin(),
                                            womenRanking[chosenWoman].end(), original_matches[1][chosenWoman]);
            if(man_difference>0){
                 if(print){
                        
                        cout<<"  "<<chosenWoman<<" matched to "<< temporary_matches[1][chosenWoman]<<" was matched to "<<original_matches[1][chosenWoman]<<"\n  Man matched to "<< temporary_matches[0][chosenMan]<<"\n";
                        // for(auto c: womenRanking[chosenWoman]){
                        //     cout<<c<<" ";
                        // }
                        // cout<<"\n ";
                    }
                    found_better=true;
                    men_with_incentive++;
                    womenRanking[chosenWoman]= initialRanking;
            }
                
            for(int swap = swapRanking+1;swap<temporaryRanking.size();swap++){
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swap);
                
                womenRanking[chosenWoman]= temporaryRanking;
                // if(print){
                //     cout<<" "<<*temporaryRanking.begin()<<" and "<<*(temporaryRanking.begin()+ swap)<<" \n";
                //     cout<<"ranking of woman is \n";
                //     for(auto c: womenRanking[chosenWoman]){
                //                 cout<<c<<" ";
                //             }
                //     cout<<"\n";

                // }
                temporary_matches = GaleShapley();
                man_difference= find(menRanking[chosenMan].begin(),
                                            menRanking[chosenMan].end(), original_matches[0][chosenMan])
                                    - find(menRanking[chosenMan].begin(), 
                                            menRanking[chosenMan].end(), temporary_matches[0][chosenMan]);
                if(man_difference>0){
                    if(print){
                        
                        cout<<"  "<<chosenWoman<<" matched to "<< temporary_matches[1][chosenWoman]<<" was matched to "<<original_matches[1][chosenWoman]<<"\n  Man matched to "<< temporary_matches[0][chosenMan]<<"\n";
                        // for(auto c: womenRanking[chosenWoman]){
                        //     cout<<c<<" ";
                        // }
                        // cout<<"\n ";
                    }
                    
                    found_better=true;
                    men_with_incentive++;
                    womenRanking[chosenWoman]= initialRanking;
                    break;
                }
            }
            womenRanking[chosenWoman]= initialRanking;
        }
        // cout<<"\n";
    }
    cout<<men_with_incentive<<"\n";
    accross_trial_men_with_incentive+= men_with_incentive;
    accross_trial_men_with_best_partner+= men_with_best_partner;

}
void generateNamesAndPreferences(bool print=false) {
    if(print)
        cout<<"Preferences\n";
    std::random_device rd;
    std::mt19937 g(rd());
    // Generate men's and women's names
    for (int i = 0; i < numberOfPeople; i++) {
        men.push_back(to_string(i+1));
        women.push_back(string(1, 'A' + i % 26));
    }
    // Generate random preferences for each man
    for (auto& man : men) {
        vector<string> preferences = women;
        shuffle(preferences.begin(), preferences.end(),g );
        menRanking[man] = preferences;
        if(print){
            cout<<man<<" : ";
            for(auto& w: preferences)
                cout<<w<<" ";
            cout<<"\n";
        }
    }
    // Generate random preferences for each woman
    for (auto& woman : women) {
        vector<string> preferences = men;
        shuffle(preferences.begin(), preferences.end(),g );
        womenRanking[woman] = preferences;
        if(print){
            cout<<woman<<" : ";
            for(auto& w: preferences)
                cout<<w<<" ";
            cout<<"\n";
        }
    }
}

vector<map<string, string>>  GaleShapley() {
    map<string, string> woman_matched;  // map woman to man
        map<string, string> man_matched;  // map woman to man
    list<string> freeMen(men.begin(), men.end());  // initially, all men are free

    while (!freeMen.empty()) {
        string currentMan = freeMen.front();
        freeMen.pop_front();

        vector<string> currentManPrefers = menRanking[currentMan];

        for (string& woman : currentManPrefers) {
            if (woman_matched.find(woman) == woman_matched.end()) {  // woman is not matched
                woman_matched[woman] = currentMan;
                man_matched[currentMan] = woman;
                break;
            } else {
                string otherMan = woman_matched[woman];
                vector<string> currentWomanRanking = womenRanking[woman];

                if (find(currentWomanRanking.begin(), currentWomanRanking.end(), currentMan) <
                    find(currentWomanRanking.begin(), currentWomanRanking.end(), otherMan)) {
                    woman_matched[woman] = currentMan;
                    man_matched[currentMan] = woman;
                    freeMen.push_back(otherMan);
                    break;
                }
            }
        }
    }

    return {man_matched,woman_matched};
}








int main(int argc, char* argv[]) {
    if(argc==2)
        numberOfPeople=atoi(argv[1]);
	freopen("results.txt", "w", stdout);
    int num_trials = 1;
    for(int i=1;i<=num_trials;i++){
        if(print)
            cout<<"Trial #"<<i<<" ";
        Trial();
        
    }
    // cout<<"accross trail :"<<accross_trial<<"\n";
    

    return 0;
}
