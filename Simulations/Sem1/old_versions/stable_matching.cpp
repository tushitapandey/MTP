#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <map>
#include <list>
#include <algorithm>
#include <random>
#include <cstdlib> 
#include <fstream>
#include<set>
using namespace std;

int numberOfPeople = 26;
// Associate each man or woman ranker with an index
void generateNamesAndPreferences(bool print, ofstream& logFile);
vector<map<string, string> >  GaleShapley(ofstream& logFile);
vector<string> men;
vector<string> women;

vector<string> initialRanking;
vector<string> temporaryRanking;
bool print = true;
int totalIdealMatches = 0;
int totalCanImprove =0;
int totalCannotImprove = 0;
int menStrictlyBetterOffByBetterWoman =0;
void print_matches(vector<map<string, string> > matching, ofstream& logFile) {

    // Print Man to Woman Matches
    // logFile << "Man to Woman Matches: " << endl;
    for (const auto& manMatch : matching[0]) {
        logFile << "Man " << manMatch.first << " : " << manMatch.second << endl;
    }
    // // Print Woman to Man Matches
    // logFile << "\nWoman to Man Matches: " << endl;
    // for (const auto& womanMatch : matching[1]) {
    //     logFile << "Woman " << womanMatch.first << " : " << womanMatch.second << endl;
    // }
}



void deal(ofstream& logFile,string man, string woman, string og_woman, vector<map<string, string> > original_matches, vector<map<string, string> > temporary_matches, vector<string> og_woman_rank, vector<string> initialRanking){
    
    string ogManWoman2, ogManWoman1, newManWoman2, newManWoman1;
    string originalWoman = original_matches[0][man];  
    string temporaryWoman = temporary_matches[0][man];  
    int man_diff, w1_diff, w2_diff;
   auto it = menRanking.find(man);
    auto originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
    auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
    int originalIndex = distance(it->second.begin(), originalPosition);
    int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
    man_diff = manipulatedIndex - originalIndex;

    ogManWoman1 = original_matches[1][og_woman];  
    newManWoman1 = temporary_matches[1][og_woman];  
    originalPosition = find(og_woman_rank.begin(), og_woman_rank.end(), ogManWoman1);
    manipulatedPosition = find(og_woman_rank.begin(), og_woman_rank.end(), newManWoman1);
    originalIndex = distance(og_woman_rank.begin(), originalPosition);
    manipulatedIndex = distance(og_woman_rank.begin(), manipulatedPosition);
    w1_diff = manipulatedIndex - originalIndex;

    ogManWoman2 = original_matches[1][woman];  
    newManWoman2 = temporary_matches[1][woman];  
    originalPosition = find(initialRanking.begin(), initialRanking.end(), ogManWoman2);
    manipulatedPosition = find(initialRanking.begin(), initialRanking.end(), newManWoman2);
    originalIndex = distance(initialRanking.begin(), originalPosition);
    manipulatedIndex = distance(initialRanking.begin(), manipulatedPosition);
    w2_diff = manipulatedIndex - originalIndex;
    if(w2_diff <=0 && w1_diff <= 0  && (w2_diff + w1_diff) <0 ){

        for(auto rman : men){
            auto iter = menRanking.find(rman);
            string roriginalWoman = original_matches[0][rman];  
            string rtemporaryWoman = temporary_matches[0][rman];  
            auto roriginalPosition = find(iter->second.begin(), iter->second.end(),roriginalWoman);
            auto rmanipulatedPosition = find(iter->second.begin(), iter->second.end(), rtemporaryWoman);
            int roriginalIndex = distance(iter->second.begin(), roriginalPosition);
            int rmanipulatedIndex = distance(iter->second.begin(), rmanipulatedPosition);
            int rman_diff = rmanipulatedIndex - roriginalIndex;
            if(rman_diff <=0){
                logFile<<"rando Woman can make other woman better off while not making man worse off"<<"\n";
                logFile<< rman <<" man " <<" difference is " << rman_diff<<"\n";
                logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
                logFile<< "For woman "<<woman<<" , difference is "<< w2_diff<<"\n";
                print_matches(temporary_matches,logFile);
            }
        }
    }
    if(man_diff <=0  && w2_diff <=0 && w1_diff <= 0){
        logFile<<"Woman can make other woman better off while not making man worse off"<<"\n";
        logFile<< man <<" man " <<" difference is " << man_diff<<"\n";
        logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
        logFile<< "For woman "<<woman<<" , difference is "<< w2_diff<<"\n";
        print_matches(temporary_matches,logFile);
        logFile<<"Preferences for the women "<< og_woman<<"\n";
            for (auto& m :og_woman_rank)
                logFile << m << " ";
            logFile<<"\n";
            for (auto& m : womenRanking[og_woman])
                logFile << m << " ";
            logFile<<"\n";
            logFile<<"Preferences for the women "<< woman<<"\n";
            for (auto& m :initialRanking)
                logFile << m << " ";
            logFile<<"\n";
            for (auto& m : womenRanking[woman])
                logFile << m << " ";
            logFile<<"\n";
    }
    if(man_diff <0  && w2_diff <=0 && w1_diff <= 0){
        logFile<<"Success!\n";
    }
    if(man_diff <0 ){
        logFile<< man <<" man " <<" difference is " << man_diff<<"\n";
        logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
        logFile<< "For woman "<<woman<<" , difference is "<< w2_diff<<"\n";
    }

}
//check if it is possible for 2 women to manipulate such that man becomes better off but no woman becomes worse off 



void woman_2(string man, string og_woman, vector<string> og_woman_rank, vector<map<string, string> > original_matches,  ofstream& logFile, ofstream& resultFile) {
    vector<map<string, string> > temporary_matches;
    for(string woman: menRanking[man]){
        if(woman==og_woman){
            continue;
        }
        else{
            vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
    //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
int swapRanking =1;
            if(swapRanking == womenRanking[woman].size()-1)   
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            temporary_matches = GaleShapley(logFile);  
            deal(logFile,man,woman,og_woman,original_matches,temporary_matches,og_woman_rank,initialRanking);
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;
                    temporary_matches = GaleShapley(logFile);
                    deal(logFile,man,woman,og_woman,original_matches,temporary_matches,og_woman_rank,initialRanking);
            }
            womenRanking[woman] = initialRanking;
        }
    }
}

void Trial_2(ofstream& logFile, ofstream& resultFile) 
{
     menRanking.clear(); womenRanking.clear();
    generateNamesAndPreferences(print, logFile);
   

    // // Men's preferences
    // menRanking["1"] = {"C", "I", "H", "F", "G", "B", "D", "E", "A"};
    // menRanking["2"] = {"I", "G", "H", "B", "E", "C", "F", "A", "D"};
    // menRanking["3"] = {"D", "A", "I", "C", "H", "E", "G", "B", "F"};
    // menRanking["4"] = {"H", "G", "C", "B", "A", "D", "E", "I", "F"};
    // menRanking["5"] = {"D", "C", "B", "E", "I", "G", "F", "A", "H"};
    // menRanking["6"] = {"B", "G", "C", "A", "F", "I", "H", "E", "D"};
    // menRanking["7"] = {"G", "E", "H", "B", "I", "F", "C", "A", "D"};
    // menRanking["8"] = {"F", "G", "C", "A", "B", "H", "D", "I", "E"};
    // menRanking["9"] = {"A", "C", "H", "D", "I", "B", "G", "F", "E"};

    // // Initialize womenRanking map
    // // map<string, vector<string>> womenRanking;

    // // Women's preferences
    // womenRanking["A"] = {"1", "5", "6", "2", "4", "7", "8", "3", "9"};
    // womenRanking["B"] = {"8", "7", "9", "1", "5", "2", "4", "3", "6"};
    // womenRanking["C"] = {"5", "7", "4", "6", "3", "2", "8", "1", "9"};
    // womenRanking["D"] = {"6", "3", "5", "8", "2", "9", "1", "4", "7"};
    // womenRanking["E"] = {"1", "3", "5", "4", "7", "9", "6", "8", "2"};
    // womenRanking["F"] = {"1", "4", "6", "2", "8", "3", "9", "7", "5"};
    // womenRanking["G"] = {"2", "9", "3", "6", "5", "4", "7", "8", "1"};
    // womenRanking["H"] = {"6", "8", "3", "7", "9", "2", "1", "5", "4"};
    // womenRanking["I"] = {"7", "5", "6", "2", "8", "9", "3", "4", "1"};

    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(logFile);
    vector<map<string, string> > temporary_matches;
    logFile<<"Original GS Matching\n";
    print_matches(original_matches,logFile);
    for(string man: men){
        // logFile<<"\nMan "<<man;
        if(original_matches[0][man]==  menRanking[man][0]){
            // logFile<<": already best";
            continue;
        }
        for(string woman: menRanking[man]){
            vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
    //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
int swapRanking =1;
            if(swapRanking == womenRanking[woman].size()-1)   
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            temporary_matches = GaleShapley(logFile);  // Get the temporary matching
            string originalWoman = original_matches[0][man];  
            string temporaryWoman = temporary_matches[0][man];  
           auto it = menRanking.find(man);
            auto originalPosition = find(it->second.begin(), it->second.end(), originalWoman);
            auto manipulatedPosition = find(it->second.begin(), it->second.end(), temporaryWoman);
            int originalIndex = distance(it->second.begin(), originalPosition);
            int manipulatedIndex = distance(it->second.begin(), manipulatedPosition);
            woman_2(man,woman,initialRanking, original_matches, logFile, resultFile);
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                womenRanking[woman] = temporaryRanking;
                woman_2(man,woman,initialRanking, original_matches, logFile, resultFile);
            }
            womenRanking[woman] = initialRanking;
        }
        
    }
}


bool printDifference(const vector<map<string, string>>& temporary_matches_new,
                     const vector<map<string, string>>& temp_matches, ofstream& logFile) {
    // Ensure both vectors have the same size
    bool ch=false;
    if (temporary_matches_new.size() != temp_matches.size()) {
        logFile << "The vectors have different sizes, unable to compare." << endl;
        ch=true;
        return ch;
    }

    // Iterate through each map at corresponding indices
    for (size_t i = 0; i < temporary_matches_new.size(); ++i) {
        const auto& new_map = temporary_matches_new[i];
        const auto& old_map = temp_matches[i];

        // logFile << "Comparing map at index " << i << ":\n";

        // Print keys and values that are in new_map but not in old_map
        for (const auto& [key, value] : new_map) {
            auto it = old_map.find(key);
            if (it == old_map.end()) {
                logFile << "  Key '" << key << "' added with value '" << value << "'\n";
    
            } else if (it->second != value) {
                logFile << "  Key '" << key << "' changed from '" << it->second
                          << "' to '" << value << "'\n";
                return true;
            }
        }

        // Print keys and values that are in old_map but not in new_map
        for (const auto& [key, value] : old_map) {
            if (new_map.find(key) == new_map.end()) {
                logFile << "  Key '" << key << "' removed, had value '" << value << "'\n";
            }
      
        }

        // logFile << "--------------------------------------\n";
    }
    return ch;
}

void printDifference(vector<string> og_ranking, string woman, ofstream& logFile){
    bool ch=false;
   for (size_t i = 0; i < og_ranking.size(); ++i) {
        // If there's a difference, log the change
        if (womenRanking[woman][i] != og_ranking[i]) {
            logFile << "Difference at index " << i << ": "
                    << "Original Ranking: " << og_ranking[i] << ", "
                    << "New Ranking for " << woman << ": " << womenRanking[woman][i] << std::endl;
            ch=true;
        }
    }
    if(ch){
        logFile<<"\n";
          for (size_t i = 0; i < og_ranking.size(); ++i) {
        logFile<<womenRanking[woman][i]<<" ";
    }
    logFile<<"\n";
    for (size_t i = 0; i < og_ranking.size(); ++i) {
        logFile<<og_ranking[i]<<" ";
    }
    
    logFile<<"\n";

    }
  


}

void deal(ofstream& logFile, string woman, string og_woman, vector<map<string, string> > original_matches, vector<map<string, string> > temp_matches,  vector<string> og_woman_rank, vector<string> initialRanking){
    
    string ogManWoman2, ogManWoman1, newManWoman2, newManWoman1;
   
    int man_diff, w1_diff, w2_diff;
    vector<map<string, string> > temporary_matches_gs = GaleShapley(logFile);
    // vector<map<string, string> > temporary_matches_old= GaleShapley(logFile);
    vector<map<string, string> > temporary_matches_new=temp_matches;
    
    // print_matches(temporary_matches_new,logFile);
    logFile<<"\n*****\n";
    print_matches(temporary_matches_gs,logFile);
    for(auto m: men){
                    logFile<<m<<": ";
                    for(auto w: menRanking[m]){
                        logFile<<w<<" ";
                    }
                    logFile<<"\n";
                }
                for(auto w: women){
                    logFile<<w<<": ";
                    for(auto m: womenRanking[w]){
                        logFile<<m<<" ";
                    }
                    logFile<<"\n";
                }
    ogManWoman2 = original_matches[1][woman];  
    newManWoman2 = temporary_matches_new[1][woman]; 
    auto originalPosition = find(initialRanking.begin(), initialRanking.end(), ogManWoman2);
    auto manipulatedPosition = find(initialRanking.begin(), initialRanking.end(), newManWoman2);
    auto originalIndex = distance(initialRanking.begin(), originalPosition);
    auto manipulatedIndex = distance(initialRanking.begin(), manipulatedPosition);
    w2_diff = manipulatedIndex - originalIndex;

    ogManWoman1 = original_matches[1][og_woman];  
    newManWoman1 = temporary_matches_new[1][og_woman];  
    originalPosition = find(og_woman_rank.begin(), og_woman_rank.end(), ogManWoman1);
    manipulatedPosition = find(og_woman_rank.begin(), og_woman_rank.end(), newManWoman1);
    originalIndex = distance(og_woman_rank.begin(), originalPosition);
    manipulatedIndex = distance(og_woman_rank.begin(), manipulatedPosition);
    w1_diff = manipulatedIndex - originalIndex;
    if(w2_diff <=0 && w1_diff <= 0){
        for(auto rman : men){
            auto iter = menRanking.find(rman);
            string roriginalWoman = original_matches[0][rman];  
            string rtemporaryWoman = temporary_matches_new[0][rman];  
            auto roriginalPosition = find(iter->second.begin(), iter->second.end(),roriginalWoman);
            auto rmanipulatedPosition = find(iter->second.begin(), iter->second.end(), rtemporaryWoman);
            int roriginalIndex = distance(iter->second.begin(), roriginalPosition);
            int rmanipulatedIndex = distance(iter->second.begin(), rmanipulatedPosition);
            int rman_diff = rmanipulatedIndex - roriginalIndex;
            if(rman_diff <0){
                logFile<<"rando Woman can make other woman better off while not making man worse off"<<"\n";
                logFile<< rman <<" man " <<" difference is " << rman_diff<<"\n";
                logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
                logFile<< "For woman "<<woman<<" , difference is "<< w2_diff<<"\n";
                // return;
                // print_matches(temporary_matches_new,logFile);
                // for(auto m: men){
                //     logFile<<m<<": ";
                //     for(auto w: menRanking[m]){
                //         logFile<<w<<" ";
                //     }
                //     logFile<<"\n";
                // }
                // for(auto w: women){
                //     logFile<<w<<": ";
                //     for(auto m: womenRanking[w]){
                //         logFile<<m<<" ";
                //     }
                //     logFile<<"\n";
                // }
                // if(printDifference(temporary_matches_new,temporary_matches_old,logFile)){
                //     logFile<<"Diff\n";
                //     printDifference(og_woman_rank,og_woman,logFile);
                //     print_matches(temporary_matches_new,logFile);
                //     print_matches(temporary_matches_old, logFile);
                //     for(auto m: men){
                //                 logFile<<m<<": ";
                //                 for(auto w: menRanking[m]){
                //                     logFile<<w<<" ";
                //                 }
                //                 logFile<<"\n";
                //             }
                //             for(auto w: women){
                //                 logFile<<w<<": ";
                //                 for(auto m: womenRanking[w]){
                //                     logFile<<m<<" ";
                //                 }
                //                 logFile<<"\n";
                // }}
                
                break;
                
                

            }
        }

    }
    logFile<<"\n\n\n*************\n\n\n";
    
}
void woman_3(string og_woman, vector<string> og_woman_rank, vector<map<string, string> > original_matches,  ofstream& logFile, ofstream& resultFile) {
    vector<map<string, string> > temporary_matches =  GaleShapley(logFile);  
    
    for(string woman: women){
        if(woman==og_woman){
            continue;
        }
        else{
            vector<string> initialRanking = womenRanking[woman];
            deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking);
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
            //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
            int swapRanking =1;
            // if(swapRanking == womenRanking[woman].size()-1)   
            //     continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  

            temporary_matches = GaleShapley(logFile);  
            logFile<<"OG FUNCTION\n";
            print_matches(temporary_matches,logFile);
            logFile<<"\n";
            for(auto m: men){
                    logFile<<m<<": ";
                    for(auto w: menRanking[m]){
                        logFile<<w<<" ";
                    }
                    logFile<<"\n";
                }
                for(auto w: women){
                    logFile<<w<<": ";
                    for(auto m: womenRanking[w]){
                        logFile<<m<<" ";
                    }
                    logFile<<"\n";
                }
            
            deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking);
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;
                    temporary_matches = GaleShapley(logFile);
                    logFile<<"OG FUNCTION\n";
                    print_matches(temporary_matches,logFile);
                    for(auto m: men){
                    logFile<<m<<": ";
                    for(auto w: menRanking[m]){
                        logFile<<w<<" ";
                    }
                    logFile<<"\n";
                }
                for(auto w: women){
                    logFile<<w<<": ";
                    for(auto m: womenRanking[w]){
                        logFile<<m<<" ";
                    }
                    logFile<<"\n";
                }
                    deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking);
            }
            womenRanking[woman] = initialRanking;
        }
    }
}


void Trial_3(ofstream& logFile, ofstream& resultFile){
    menRanking.clear(); womenRanking.clear();
    generateNamesAndPreferences(print, logFile);
    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(logFile);
    vector<map<string, string> > temporary_matches;
    logFile<<"Original GS Matching\n";
    print_matches(original_matches,logFile);
    for(string woman: women){
        vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
    //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
        int swapRanking =1;
          
            if(swapRanking == womenRanking[woman].size()-1)   
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            woman_3(woman,initialRanking, original_matches, logFile, resultFile);
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                womenRanking[woman] = temporaryRanking;
                woman_3(woman,initialRanking, original_matches, logFile, resultFile);
            }
            womenRanking[woman] = initialRanking;
    }

}


void galeShapleyOnCustom(ofstream& logFile){
// Men's preferences
menRanking["1"] = {"B", "H", "I", "F", "E", "A", "C", "D", "G"};
menRanking["2"] = {"H", "G", "C", "A", "I", "E", "F", "D", "B"};
menRanking["3"] = {"G", "B", "D", "A", "C", "E", "H", "F", "I"};
menRanking["4"] = {"H", "B", "C", "F", "D", "G", "E", "A", "I"};
menRanking["5"] = {"E", "C", "D", "B", "A", "G", "I", "H", "F"};
menRanking["6"] = {"I", "H", "C", "G", "D", "B", "E", "A", "F"};
menRanking["7"] = {"I", "G", "D", "E", "A", "F", "H", "B", "C"};
menRanking["8"] = {"E", "I", "H", "F", "B", "A", "D", "G", "C"};
menRanking["9"] = {"B", "H", "E", "C", "I", "F", "D", "A", "G"};


// Women's preferences
womenRanking["A"] = {"3", "4", "9", "8", "7", "2", "5", "6", "1"};
womenRanking["B"] = { "8", "9","5", "4", "2", "1", "6", "3", "7"};
womenRanking["C"] = {"7", "6", "4", "5", "1", "8", "3", "9", "2"};
womenRanking["D"] = {"9", "8", "7", "1", "4", "5", "2", "6", "3"};
womenRanking["E"] = {"5", "6", "2", "4", "1", "9", "8", "3", "7"};
womenRanking["F"] = {"8", "9", "4", "6", "7", "2", "3", "1", "5"};
womenRanking["G"] = {"7", "6", "1", "2", "3", "5", "8", "4", "9"};
womenRanking["H"] = {"5", "2", "6", "3", "9", "7", "8", "4", "1"};
womenRanking["I"] = {"1", "5", "7", "2", "4", "6", "8", "3", "9"};


vector<map<string, string> > original_matches ;
vector<map<string, string> > t_matches ;
original_matches= GaleShapley(logFile);
print_matches(original_matches,logFile);
}

void generateNamesAndPreferences(bool pref_print, ofstream& logFile) {
   
    if (pref_print)
        logFile << "\nPreferences\n";
    random_device rd;
    mt19937 g(rd());
    for (auto& man : men) {
        vector<string> preferences = women;
        shuffle(preferences.begin(), preferences.end(), g);
        menRanking[man] = preferences;
        if (pref_print) {
            logFile << man << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
    for (auto& woman : women) {
        vector<string> preferences = men;
        shuffle(preferences.begin(), preferences.end(), g);
        womenRanking[woman] = preferences;
        if (pref_print) {
            logFile << woman << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
}

vector<map<string, vector<string> > > generateNamesAndPreferences(bool pref_print, ofstream& logFile) {
    map<string, vector<string> > menRanking;
    map<string, vector<string> > womenRanking;
    if (pref_print)
        logFile << "\nPreferences\n";
    random_device rd;
    mt19937 g(rd());
    for (auto& man : men) {
        vector<string> preferences = women;
        shuffle(preferences.begin(), preferences.end(), g);
        menRanking[man] = preferences;
        if (pref_print) {
            logFile << man << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
    for (auto& woman : women) {
        vector<string> preferences = men;
        shuffle(preferences.begin(), preferences.end(), g);
        womenRanking[woman] = preferences;
        if (pref_print) {
            logFile << woman << " : ";
            for (auto& w : preferences)
                logFile << w << " ";
            logFile << "\n";
        }
    }
    return { menRanking, wommenRanking };
}

vector<map<string, string>>  GaleShapley(ofstream& logFile) {
    map<string, string> woman_matched;  // map woman to man
    map<string, string> man_matched;    // map man to woman
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

    return { man_matched, woman_matched };
}

vector<map<string, string>> GaleShapley(const map<string, vector<string>>& menRanking,
                                         const map<string, vector<string>>& womenRanking,
                                         ofstream& logFile) {
    map<string, string> woman_matched;  // map woman to man
    map<string, string> man_matched;    // map man to woman
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

    return { man_matched, woman_matched };
}

void deal(ofstream& logFile, string woman, string og_woman, vector<map<string, string> > original_matches, vector<map<string, string> > temp_matches,  vector<string> og_woman_rank, vector<string> initialRanking,vector<string>> menRanking,
                                          map<string, vector<string>> womenRanking){
    vector<map<string, string> > temporary_matches_new=temp_matches;
    string ogManWoman2, ogManWoman1, newManWoman2, newManWoman1;
   
    int man_diff, w1_diff, w2_diff;
    // vector<map<string, string> > temporary_matches_gs = GaleShapley(logFile);
    // vector<map<string, string> > temporary_matches_old= GaleShapley(logFile);
    
    
    // print_matches(temporary_matches_new,logFile);
    // logFile<<"\n*****\n";
    // print_matches(temporary_matches_gs,logFile);
    // for(auto m: men){
    //                 logFile<<m<<": ";
    //                 for(auto w: menRanking[m]){
    //                     logFile<<w<<" ";
    //                 }
    //                 logFile<<"\n";
    //             }
    //             for(auto w: women){
    //                 logFile<<w<<": ";
    //                 for(auto m: womenRanking[w]){
    //                     logFile<<m<<" ";
    //                 }
    //                 logFile<<"\n";
    //             }
    ogManWoman2 = original_matches[1][woman];  
    newManWoman2 = temporary_matches_new[1][woman]; 
    auto originalPosition = find(initialRanking.begin(), initialRanking.end(), ogManWoman2);
    auto manipulatedPosition = find(initialRanking.begin(), initialRanking.end(), newManWoman2);
    auto originalIndex = distance(initialRanking.begin(), originalPosition);
    auto manipulatedIndex = distance(initialRanking.begin(), manipulatedPosition);
    w2_diff = manipulatedIndex - originalIndex;

    ogManWoman1 = original_matches[1][og_woman];  
    newManWoman1 = temporary_matches_new[1][og_woman];  
    originalPosition = find(og_woman_rank.begin(), og_woman_rank.end(), ogManWoman1);
    manipulatedPosition = find(og_woman_rank.begin(), og_woman_rank.end(), newManWoman1);
    originalIndex = distance(og_woman_rank.begin(), originalPosition);
    manipulatedIndex = distance(og_woman_rank.begin(), manipulatedPosition);
    w1_diff = manipulatedIndex - originalIndex;
    if(w2_diff <=0 && w1_diff <= 0){
        for(auto rman : men){
            auto iter = menRanking.find(rman);
            string roriginalWoman = original_matches[0][rman];  
            string rtemporaryWoman = temporary_matches_new[0][rman];  
            auto roriginalPosition = find(iter->second.begin(), iter->second.end(),roriginalWoman);
            auto rmanipulatedPosition = find(iter->second.begin(), iter->second.end(), rtemporaryWoman);
            int roriginalIndex = distance(iter->second.begin(), roriginalPosition);
            int rmanipulatedIndex = distance(iter->second.begin(), rmanipulatedPosition);
            int rman_diff = rmanipulatedIndex - roriginalIndex;
            if(rman_diff <0){
                logFile<<"rando Woman can make other woman better off while not making man worse off"<<"\n";
                logFile<< rman <<" man " <<" difference is " << rman_diff<<"\n";
                logFile<< "For woman "<<og_woman<<" , difference is "<< w1_diff<<"\n";
                logFile<< "For woman "<<woman<<" , difference is "<< w2_diff<<"\n";
                // return;
                // print_matches(temporary_matches_new,logFile);
                // for(auto m: men){
                //     logFile<<m<<": ";
                //     for(auto w: menRanking[m]){
                //         logFile<<w<<" ";
                //     }
                //     logFile<<"\n";
                // }
                // for(auto w: women){
                //     logFile<<w<<": ";
                //     for(auto m: womenRanking[w]){
                //         logFile<<m<<" ";
                //     }
                //     logFile<<"\n";
                // }
                // if(printDifference(temporary_matches_new,temporary_matches_old,logFile)){
                //     logFile<<"Diff\n";
                //     printDifference(og_woman_rank,og_woman,logFile);
                //     print_matches(temporary_matches_new,logFile);
                //     print_matches(temporary_matches_old, logFile);
                //     for(auto m: men){
                //                 logFile<<m<<": ";
                //                 for(auto w: menRanking[m]){
                //                     logFile<<w<<" ";
                //                 }
                //                 logFile<<"\n";
                //             }
                //             for(auto w: women){
                //                 logFile<<w<<": ";
                //                 for(auto m: womenRanking[w]){
                //                     logFile<<m<<" ";
                //                 }
                //                 logFile<<"\n";
                // }}
                
                break;
                
                

            }
        }

    }
    // logFile<<"\n\n\n*************\n\n\n";
    
}
void woman_3(string og_woman, vector<string> og_woman_rank, vector<map<string, string> > original_matches,  ofstream& logFile, ofstream& resultFile, map<string, vector<string>> menRanking,
                                          map<string, vector<string>> womenRanking){
    vector<map<string, string> > temporary_matches =  GaleShapley(logFile);  
    
    for(string woman: women){
        if(woman==og_woman){
            continue;
        }
        else{
            vector<string> initialRanking = womenRanking[woman];
            deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking,menRanking,womenRanking);
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
            //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
            int swapRanking =1;
            // if(swapRanking == womenRanking[woman].size()-1)   
            //     continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  

            temporary_matches = GaleShapley(logFile);  
            logFile<<"OG FUNCTION\n";
            print_matches(temporary_matches,logFile);
            logFile<<"\n";
            for(auto m: men){
                    logFile<<m<<": ";
                    for(auto w: menRanking[m]){
                        logFile<<w<<" ";
                    }
                    logFile<<"\n";
                }
                for(auto w: women){
                    logFile<<w<<": ";
                    for(auto m: womenRanking[w]){
                        logFile<<m<<" ";
                    }
                    logFile<<"\n";
                }
            
            deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking,menRanking,womenRanking);
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                    iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                    womenRanking[woman] = temporaryRanking;
                    temporary_matches = GaleShapley(logFile);
                    logFile<<"OG FUNCTION\n";
                    print_matches(temporary_matches,logFile);
                    for(auto m: men){
                    logFile<<m<<": ";
                    for(auto w: menRanking[m]){
                        logFile<<w<<" ";
                    }
                    logFile<<"\n";
                }
                for(auto w: women){
                    logFile<<w<<": ";
                    for(auto m: womenRanking[w]){
                        logFile<<m<<" ";
                    }
                    logFile<<"\n";
                }
                    deal(logFile,woman,og_woman,original_matches,temporary_matches,og_woman_rank, initialRanking,menRanking,womenRanking);
            }
            womenRanking[woman] = initialRanking;
        }
    }
}

void Trial_3(ofstream& logFile, ofstream& resultFile, map<string, vector<string>> menRanking,
                                          map<string, vector<string>> womenRanking){
    vector<map<string, string> > original_matches ;
    original_matches= GaleShapley(logFile);
    vector<map<string, string> > temporary_matches;
    logFile<<"Original GS Matching\n";
    print_matches(original_matches,logFile);
    for(string woman: women){
        vector<string> initialRanking = womenRanking[woman];
            auto iterator= find(womenRanking[woman].begin(), womenRanking[woman].end(), original_matches[1][woman]);
    //int swapRanking =  distance(womenRanking[woman].begin(), iterator) ;
        int swapRanking =1;
          
            if(swapRanking == womenRanking[woman].size()-1)   
                continue;
            vector<string> temporaryRanking = initialRanking;
            string swapMan = temporaryRanking[swapRanking];
            temporaryRanking.erase(temporaryRanking.begin() + swapRanking++);
            temporaryRanking.insert(temporaryRanking.begin(), swapMan);
            womenRanking[woman] = temporaryRanking;  
            woman_3(woman,initialRanking, original_matches, logFile, resultFile,menRanking,womenRanking);
            for(; swapRanking< temporaryRanking.size();swapRanking++){
                iter_swap(temporaryRanking.begin(), temporaryRanking.begin() + swapRanking);
                womenRanking[woman] = temporaryRanking;
                woman_3(woman,initialRanking, original_matches, logFile, resultFile,menRanking,womenRanking);
            }
            womenRanking[woman] = initialRanking;
    }

}

int main(int argc, char* argv[]) {
    if (argc == 2)
        numberOfPeople = atoi(argv[1]);

    ofstream resultFile("results.txt");
    ofstream logFile("log_file.txt");
    ofstream finalResultFile("final_results.txt");
    int numberOfPeople = 9;
    for (int i = 0; i < numberOfPeople; i++) {
        men.push_back(to_string(i + 1));  // Men are still numbered from 1 to 10
        women.push_back(string(1, 'A' + i));  // Women are assigned letters from 'A' to 'J'
    }
    int trials =10;
    vector<map<string, vector<string> > > m = generateNamesAndPreferences(false,logFile);
    for(int i=0; i<trials;i++){
        logFile<<"Trial #"<<i<<"\n";
        resultFile<<"Trial #"<<i<<"\n";
        Trial_3(logFile,resultFile,m[0],m[1]);
        // galeShapleyOnCustom(logFile);
        logFile<<"\n\n";
        
        
    }
    resultFile<<"\n\n";
        finalResultFile << "After " << trials  <<" trials:\n";
        finalResultFile << "Total Metric 1: " << 100.0*totalIdealMatches/(numberOfPeople*trials) << "% men matched with their most ideal partner.\n";
        finalResultFile << "Total Metric 2: " << 100.0*totalCannotImprove/(numberOfPeople*trials) << "% men cannot improve by manipulating a woman lower in their ranking than the match they got.\n";
        finalResultFile << "Total Metric 3: " << 100.0*totalCanImprove/(numberOfPeople*trials) << "% men can improve by manipulating a woman lower in their ranking than the match they got.\n";
        finalResultFile << "Total Metric 4: " << 100.0*menStrictlyBetterOffByBetterWoman /(numberOfPeople*trials) << "% men can improve to a woman higher than the lying woman in their ranking.\n";
    return 0;
}
