/**
* generate_var.cpp - Program for generating variation likelihoods and data
* Author: Zeno Marquis
* HW5: Naive Bayesian Classification
* 4/25/2024
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

ifstream open_file(string filename);
void read_to_file(vector<float> bird_likelihoods, vector<float> plane_likelihoods, ofstream &outfile);
void generate_variation_data(ifstream &data, ofstream &var_data);
void generate_variation_likelihoods(ifstream &data, ofstream &var_likelihoods);

/**
     * Main function for generating variation data
     * 
     * @param argc number of arguments
     * @param argv the arguments of the program
     */
int main(int argc, char **argv){

    //program dataset test variation_likelihoods test_variation
    if (argc != 5){
        cerr << "Error: wrong number of arguments provided (5 required, ";
        cerr << argc << " provided\n";
        exit(EXIT_FAILURE);
    }

    //get training data and determine likelihoods from it
    ifstream training_data = open_file(argv[1]);
    ofstream var_likelihoods;
    var_likelihoods.open(argv[4]);

    generate_variation_likelihoods(training_data, var_likelihoods);

    var_likelihoods.close();
    training_data.close();

    //get test data and create variation data from it
    ifstream test_data = open_file(argv[2]);
    ofstream var_data;
    var_data.open(argv[3]);

    generate_variation_data(test_data, var_data);

    var_data.close();
    test_data.close();

    return 0;
}

/**
     * Opens the given input file
     * 
     * @param filename a string representing the input file name
     * 
     * @return the input file stream generated from opening the file
     */
ifstream open_file(string filename){

    //make sure input file opens
    ifstream infile;
    infile.open(filename); 
    if (infile.fail()) {
        cerr << "ERROR: Error opening file " << filename;
        exit(EXIT_FAILURE);
    } else {
        return infile;
    }
}

/**
     * Reads data to the given output file
     * 
     * @param bird_likelihoods the variation likelihoods for birds
     * @param plane_likelihoods the variation likelihoods for planes
     * @param outfile the output file stream
     */
void read_to_file(vector<float> bird_likelihoods, vector<float> plane_likelihoods, ofstream &outfile){

    for (int i = 0; i < 100; i++){
        outfile << bird_likelihoods[i] << " ";
    }

    outfile << endl;

    for (int i = 0; i < 100; i++){
        outfile << plane_likelihoods[i] << " ";
    } 

    outfile << endl;

}

/**
     * Generates variation data and prints it to the given output file
     * 
     * @param data the input data stream
     * @param var_data the output file where variation data is stored
     */
void generate_variation_data(ifstream &data, ofstream &var_data){

    for (int i = 0; i < 10; i++){

        vector<int> object_data;
        string tempstring;
        stringstream ss;

        float curr_value;
        float last_value = 0.0;

        string line;
        getline(data, line);
        ss << line;

        while (!ss.eof()){

            ss >> tempstring;

            if (tempstring.at(tempstring.size() - 1) != '\n'){
                if (tempstring != "NaN"){

                    curr_value = stof(tempstring);

                    if (last_value != 0.0){
                        //variation = abs(current speed - speed 1 second ago)
                        int difference = abs(curr_value - last_value);
                        object_data.push_back(difference);
                    } else {
                        //inserts -1 when variation could not be calculated
                        object_data.push_back(-1);
                    }
                } else {
                    object_data.push_back(-1);
                    curr_value = 0.0;
                }

                last_value = curr_value;

            }
        }

        //write variation data to the output file
        int num_objects = object_data.size();
        for (int j = 0; j < num_objects; j++){
        
            var_data << object_data.at(j);
            if (j != num_objects - 1){
                var_data << ' ';
            }
        }

        var_data << endl;

    }
}

/**
     * Generates likelihood functions for speed variation
     * 
     * @param data training data as an input file stream
     * @param var_likelihoods the output file for the calculated likelihoods
     */
void generate_variation_likelihoods(ifstream &data, ofstream &var_likelihoods){

    vector<float> bird_likelihoods;
    vector<float> plane_likelihoods;

    //start off with a blank array from 0-100
    for (int i = 0; i < 100; i++){
        bird_likelihoods.push_back(0.0);
        plane_likelihoods.push_back(0.0);
    }

    int bird_datapoints = 0;
    int plane_datapoints = 0;

    //iterate through all 20 training data objects
    for (int i = 0; i < 20; i++){

        string tempstring;

        stringstream ss;

        float curr_value;
        float last_value = 0.0;

        string line;
        getline(data, line);
        ss << line;

        while (!ss.eof()){

            ss >> tempstring;

            if (tempstring.at(tempstring.size() - 1) != '\n'){
                if (tempstring != "NaN"){

                    curr_value = stof(tempstring);

                    if (last_value != 0.0){

                        //calculate the difference and add it to a distribution
                        int difference = abs(curr_value - last_value);
                        if (i <= 9){
                            bird_datapoints++;
                            bird_likelihoods[difference]++;
                        } else {   
                            plane_datapoints++;
                            plane_likelihoods[difference]++;
                        }
                    }
                } else {
                    curr_value = 0.0;
                }

                last_value = curr_value;

            }
        }
    }

    //turns prevalence into likelihood by diving by number of data points
    for (int i = 0; i < 100; i++){

            bird_likelihoods[i] = bird_likelihoods[i]/bird_datapoints;
            plane_likelihoods[i] = plane_likelihoods[i]/plane_datapoints;
        
    }

    read_to_file(bird_likelihoods, plane_likelihoods, var_likelihoods);

}
