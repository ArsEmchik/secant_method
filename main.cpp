#include <iomanip>
#include <iostream>
#include <math.h>
#include <time.h>
using namespace std;

#define MAX_ITER 10000 // max number of iterations

float e; // precision
float h; // step

struct Interval {
	float start;
	float end;
	Interval* next;
};

/////////////////////////////////////////////////
//                FUNCTIONS
/////////////////////////////////////////////////
float func_1(float x) {
	//return cos(x);
	return -1*(0.5 + x*x - cos(x));
}

float func_2(float x) {
	return (-sin(x) + 0.5);
}

float func_3(float x) {
	return x*x*x + 2;
}

float (*func_ptr)(float);
/////////////////////////////////////////////////



/////////////////////////////////////////////////
//                INTERVALS SPLIT
/////////////////////////////////////////////////
Interval* get_intervals(float (*func_ptr)(float), float a, float b) {
	bool find_start_value = false;
	Interval* first_interval =  NULL;
	Interval* last_interval = NULL;


	for(float i = a; i < b; i += h) {
		if(fabs(func_ptr(i)) < e && !find_start_value)
		{
			if(first_interval == NULL)
			{
				first_interval = new Interval;
				last_interval = first_interval;
			}
			else
			{
				last_interval->next = new Interval;
				last_interval = last_interval->next;				
			}

			last_interval->next = NULL;
			last_interval->start = i;
			find_start_value = true;
		}		

		if(abs(func_ptr(i)) > e && find_start_value)
		{
			last_interval->end = i;	
			find_start_value = false;

			//debug string
			//cout << endl << "Start " << last_interval->start << "/ End: " << last_interval->end << endl;
		}
	}

	// return NULL if only find start and dont find end of the interval
	return find_start_value ? NULL : first_interval;
}
/////////////////////////////////////////////////



/////////////////////////////////////////////////
//               SECAND ALGORITHM
/////////////////////////////////////////////////
float secant_method(float (*func_ptr)(float), float a, float b, int& iteration_count) {   
	float x0 = a;
	float x1 = b;	
	float new_x0;

	unsigned int cycle_count = 0;
	
	while(fabs(x1 - x0) > e && cycle_count++ < MAX_ITER) 
	{  
		new_x0 = x1;
		x1 = x1 - (x1 - x0) * func_ptr(x1)/(func_ptr(x1) - func_ptr(x0));
		x0 = new_x0;
		iteration_count++;
	}

	return x1;	
}

void secant_method_run(float (*func_ptr)(float), Interval* interval_ptr) {
	int iteration_count = 0;
	cout << endl << "=============== SECAND METHOD  ===============" << endl << endl
		<< setw(20) << "Root" << setw(2) << "|" << setw(20) << "Func value" << setw(2) << " |" << endl;

	clock_t start = clock();

	while(interval_ptr != NULL) {
		if(func_ptr(interval_ptr->start) * func_ptr(interval_ptr->end) >= 0)
		{
			interval_ptr = interval_ptr->next;
			continue;
		}

		float root = secant_method(func_ptr, interval_ptr->start, interval_ptr->end, iteration_count);
		cout << setw(20) << root << setw(2) << "|" << setw(20) << func_ptr(root) << " |"  << endl;
		interval_ptr = interval_ptr->next;
	}	

	clock_t end = clock();
	cout << "Iteration count: " << iteration_count << " Time: " << end - start << "ms" << endl << endl;
}
/////////////////////////////////////////////////


/////////////////////////////////////////////////
//               VEGSTEYN ALGORITHM
/////////////////////////////////////////////////
float vegsteyn_method(float (*func_ptr)(float), float a, float b, int& iteration_count) {   
	float x0 = a;
	float x1 = b;

	float y0 = func_ptr(a);
	float y1 = func_ptr(b);

	float root, diff;
	unsigned int cycle_count = 0;

	do {
		root = x1 - y1*(x1 - x0)/(y1 - y0);
		diff = fabs(x1 - root);

		x0 = x1;
		x1 = root;
		y0 = y1;
		y1 = func_ptr(root);

		iteration_count++;
		cycle_count++;
	}while(diff > e && cycle_count < MAX_ITER);

	return root;
}

void vegsteyn_method_run(float (*func_ptr)(float), Interval* interval_ptr) {
	int iteration_count = 0;
	cout << endl << "=============== VEGSTEYN METHOD  ===============" << endl << endl
		<< setw(20) << "Root" << setw(2) << "|" << setw(20) << "Func value" << setw(2) << " |" << endl;

	clock_t start = clock();

	while(interval_ptr != NULL) {
		if(func_ptr(interval_ptr->start) * func_ptr(interval_ptr->end) >= 0)
		{
			interval_ptr = interval_ptr->next;
			continue;
		}

		float root = vegsteyn_method(func_ptr, interval_ptr->start, interval_ptr->end, iteration_count);
		cout << setw(20) << root << setw(2) << "|" << setw(20) << func_ptr(root) << " |"  << endl;
		interval_ptr = interval_ptr->next;
	}	


	clock_t end = clock();
	cout << "Iteration count: " << iteration_count << " Time: " << end - start << "ms" << endl << endl;
}
/////////////////////////////////////////////////


char get_key() {
	cout << "MENU" << endl 
		<< "1 - func 1" << endl
		<< "2 - func 2" << endl
		<< "3 - func 3" << endl
		<< "q - exit" << endl
		<< ">> ";

	char c; cin >> c;
	return c;
}

Interval enter_interval() {
	float a, b;

	do{
		cout << endl 
			<< "Enter interval:" << endl
			<< "a> "; cin >> a;
		cout << "b> "; cin >> b;
	}while(a >= b);

	Interval interval; 
	interval.start = a; interval.end = b;
	return interval;
}

bool run() {
	bool func_chosen = false;

	while(!func_chosen) {
		switch(get_key()){
		case '1': func_ptr = func_1; func_chosen = true; break;
		case '2': func_ptr = func_2; func_chosen = true; break;
		case '3': func_ptr = func_3; func_chosen = true; break;
		case 'q': return false;
		}
	}

	cout << "Enter precision >> "; cin >> e;
	h = e/10; // step

	// enter base interval and func split to little intervals :)	
	Interval base_interval = enter_interval();
	Interval* intervals = get_intervals(func_ptr, base_interval.start, base_interval.end);


	vegsteyn_method_run(func_ptr, intervals);
	secant_method_run(func_ptr, intervals);

	return true;
}

void main() {	
	while(run());
}