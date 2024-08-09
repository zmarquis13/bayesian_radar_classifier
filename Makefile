##Builds full executable for the program
Bayesian: main.cpp
	g++ -std=c++20 -Wall -Wextra main.cpp -o Bayesian

Generate: generate_var.cpp
	g++ -std=c++20 -Wall -Wextra generate_var.cpp -o Generate