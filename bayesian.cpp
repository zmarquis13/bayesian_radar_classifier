/*
* bayesian.cpp - classification program for Naive Bayesian project
* Author: Zeno Marquis
* HW5: Naive Bayesian Classification
* 4/25/2024
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>


using namespace std;


ifstream open_file(string filename);
float bayes_bird(vector<float> speed_data, vector<float> bird_speed_likelihoods, 
vector<float> plane_speed_likelihoods, float prior);
float bayes_plane(vector<float> speed_data, vector<int> object_variation_data, vector<float> bird_speed_likelihoods, 
vector<float> plane_speed_likelihoods, 
vector<float> bird_variation_likelihoods, 
vector<float> plane_variation_likelihoods, float prior);
void get_data(ifstream *speed_data, ifstream *variation_data, vector<float> bird_speed_likelihoods, vector<float> plane_speed_likelihoods, 
vector<float> bird_variation_likelihoods, 
vector<float> plane_variation_likelihoods, int n);
void print_message(float plane_probability, float bird_probability, int count);


/**
     * Main function for classification program
     * 
     * @param argc number of arguments
     * @param argv the arguments of the program
     */
int main(int argc, char **argv){

    //5 arguments expected:
    //program dataset likelihood variation_data variation_likelihood
    if (argc != 5){
        cerr << "Error: wrong number of arguments provided (5 required, ";
        cerr << argc << " provided\n";
        exit(EXIT_FAILURE);
    }

    ifstream speed_data = open_file(argv[1]);
    ifstream speed_likelihoods = open_file(argv[2]);
    ifstream variation_data = open_file(argv[3]);    
    ifstream variation_likelihoods = open_file(argv[4]);

    vector<float> bird_speed_likelihoods;
    vector<float> plane_speed_likelihoods;

    float temp;
    string tempstring;

    //read from file to likelihoods
    while (!speed_likelihoods.eof() && speed_likelihoods.peek()!='\n'){
        speed_likelihoods >> temp;
        bird_speed_likelihoods.push_back(temp);
    }

    //passes over newline
    speed_likelihoods >> temp;

    while (!speed_likelihoods.eof()){
        speed_likelihoods >> temp;
        plane_speed_likelihoods.push_back(temp);
    }

    vector<float> bird_variation_likelihoods;
    vector<float> plane_variation_likelihoods;

    stringstream ss;

    string line;
    getline(variation_likelihoods, line);
    ss << line;

    while (!ss.eof()){
        ss >> temp;
        bird_variation_likelihoods.push_back(temp);  
    }

    while (!variation_likelihoods.eof()){
        variation_likelihoods >> temp;
        plane_variation_likelihoods.push_back(temp);
    }

    //run algorithm for all 10 objeecrts
    for (int i = 0; i < 10; i++){
        get_data(&speed_data, &variation_data, bird_speed_likelihoods,
        plane_speed_likelihoods, bird_variation_likelihoods,
        plane_variation_likelihoods, i + 1);
    }
    
    speed_data.close();
    speed_likelihoods.close();
    variation_data.close(); 
    variation_likelihoods.close(); 

    return 0;
}

/**
     * Opens the given input file
     * 
     * @param filename a string representing the input file name
     * 
     * @return the input file stream generated from opening the file
     */
ifstream open_file(string filename)
{

    //make sure file opens
    ifstream infile;
    infile.open(filename); 
    if (infile.fail()) {
        cerr << "ERROR: Error opening file";
        exit(EXIT_FAILURE);
    } else {
        return infile;
    }
}

/**
     * Runs recursive Bayesian classification based on speed data to determine
     * how likely the object is to be a bird
     * 
     * @param object_speed_data a vector representing the speed of the object
     * over time
     * @param bird_speed_likelihoods a vector representing the likelihood of
     * a bird recording any given speed
     * @param plane_speed_likelihoods a vector representing the likelihood of
     * a plane recording any given speed
     * 
     * @return a float representing the calculated probability of the given
     * object being a bird
     */
float bayes_bird(vector<float> object_speed_data, 
vector<float> bird_speed_likelihoods,
vector<float> plane_speed_likelihoods, float prior)
{

    float posterior = prior;
    int data_points = object_speed_data.size();

    for (int i = 1; i < data_points; i++){

        //prob of observing the evidence: p(bird)*p(e|b) + p(plane)*p(e|p)
        float prob_evidence = prior*bird_speed_likelihoods[2*object_speed_data[i]] + (1.0 - prior)*plane_speed_likelihoods[2*object_speed_data[i]];

        posterior = bird_speed_likelihoods[2*object_speed_data[i]]*prior/prob_evidence;
        prior = posterior;

    }

    return posterior;
}

/**
     * Runs recursive Bayesian classification based on speed data to determine
     * how likely the object is to be a plane
     * 
     * @param object_speed_data a vector representing the speed of the object
     * over time
     * @param bird_speed_likelihoods a vector representing the likelihood of
     * a bird recording any given speed
     * @param plane_speed_likelihoods a vector representing the likelihood of
     * a plane recording any given speed
     * @param bird_variation_likelihoods a vector representing the likelihood of
     * a bird having any given speed variation value
     * @param plane_variation_likelihoods a vector representing the likelihood
     * of a plane recording any given speed variation value
     * @param prior a float representing the initial prior probability of the
     * object being a plane
     * 
     * @return a float representing the calculated probability of the given
     * object being a plane
     */
float bayes_plane(vector<float> object_speed_data, vector<int> object_variation_data, vector<float> bird_speed_likelihoods, 
vector<float> plane_speed_likelihoods, 
vector<float> bird_variation_likelihoods, 
vector<float> plane_variation_likelihoods, float prior)
{
    
    float posterior;
    int data_points = object_speed_data.size();
    
    for (int i = 1; i < data_points; i++){
        
        //check if data is available at the given time
        if (object_speed_data[i] != -1 && object_variation_data[i] != -1){
            
            //probability of observing the evidence is (p(plane)*p(e|p) + p(bird)*p(e|b))
            float prob_evidence = (prior * plane_variation_likelihoods[object_variation_data[i]] + (1.0 - prior) * bird_variation_likelihoods[object_variation_data[i]]) * (prior * plane_speed_likelihoods[2*object_speed_data[i]] + (1.0 - prior) * bird_speed_likelihoods[2*object_speed_data[i]]);

            float p_e_given_plane = plane_speed_likelihoods[2*object_speed_data[i]] * plane_variation_likelihoods[object_variation_data[i]];

            //Bayes' law
            posterior = p_e_given_plane*prior/prob_evidence;
            prior = posterior;

        } else if (object_speed_data[i] != -1){

            //calculate prob_evidence based only on speed
            float prob_evidence = (1 - prior)*bird_speed_likelihoods[2*object_speed_data[i]] + prior*plane_speed_likelihoods[2*object_speed_data[i]];

            posterior = prior * plane_speed_likelihoods[2*object_speed_data[i]]/prob_evidence;
        }
    }

    return posterior;

}

/**
     * Reads file data into vectors and calls the classification functions
     * 
     * @param speed_data an input file stream containing object speed data
     * @param variation_data an input file stream containing object variation
     * @param plane_speed_likelihoods a vector representing the likelihood of
     * a plane recording any given speed
     * @param bird_speed_likelihoods a vector representing the likelihood of
     * a bird recording any given speed
     * @param plane_variation_likelihoods a vector representing the likelihood
     * of a plane recording any given speed variation value
     * @param bird_variation_likelihoods a vector representing the likelihood
     * of a bird recording any given speed variation value
     * @param n an int representing the object number
     */
void get_data(ifstream *speed_data, ifstream *variation_data, 
vector<float> bird_speed_likelihoods, vector<float> plane_speed_likelihoods, 
vector<float> bird_variation_likelihoods, 
vector<float> plane_variation_likelihoods, int n)
{
    vector<float> object_speed_data;

    
    stringstream ss;
    string line;

    getline(*speed_data, line);
    ss << line;

    string tempstring;

    //read from stringstream to vector
    while (!ss.eof()){

        ss >> tempstring;

        if (tempstring.at(tempstring.size() - 1) != '\n'){
            //check for NaN values and record them as -1
            if (tempstring != "NaN"){
                object_speed_data.push_back(stof(tempstring));
            } else {
                object_speed_data.push_back(-1);
            }
        }        
    }


    vector<int> object_variation_data;
    stringstream ss2;

    getline(*variation_data, line);
    ss2 << line;

    int tempint;

    //read from variation data to vector
    while (!ss2.eof()){
        ss2 >> tempint;
        object_variation_data.push_back(tempint);        
    }

    //calculate probability of the object being a bird or a plane
    float bird_probability = bayes_bird(object_speed_data, bird_speed_likelihoods, plane_speed_likelihoods, 0.5);

    float plane_probability = bayes_plane(object_speed_data, object_variation_data, bird_speed_likelihoods, plane_speed_likelihoods, bird_variation_likelihoods, plane_variation_likelihoods, 0.5);
    
    print_message(plane_probability, bird_probability, n);

}

/**
     * Prints the result of the calculations to the user for one object
     * 
     * @param plane_probability a float representing the probability of the
     * object being a plane
     * @param bird_probability a float representing the probability of the
     * object being a bird
     * @param n an int representing the object number
     */
void print_message(float plane_probability, float bird_probability, int count){

    int result = 0;

    //if plane claculation ran into a data point out of likelihood range, use
    //bird probability instead
    if (isnan(plane_probability)){
        cout << "Probability of object " << count << " being a plane: ";
        cout << 1 - bird_probability << endl;

        cout << "Probability of object " << count << " being a bird: " ;
        cout << bird_probability << endl;

        if (bird_probability >= 0.5){
            result = 1;
        }

    } else {
        cout << "Probability of object " << count << " being a plane: " << plane_probability << endl;

        cout << "Probability of object " << count << " being a bird: " ;
        cout << 1.0 - plane_probability << endl;

        if (plane_probability <= 0.5){
            result = 1;
        }
    }

    if (result == 1){
        cout << "Conclusion: Object " << count << " is a bird\n";
    } else {
        cout << "Conclusion: Object " << count << " is a plane\n";
    }

}
