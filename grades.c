#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "grades.h"
#include "linked-list.h"
#define FAILURE -1
#define SUCCSESS 0


struct student{
	int id;
	char* name;
	struct list *courses;
};

//list of students
struct grades{
	struct list* students;
};

struct course{
	char* course_name;
	int grade;
};

int clone_course(void *element, void **output){
	struct course* course_org = (struct course*)element;
	struct course* course_cpy = (struct course*)malloc(sizeof(struct course));
	if(!course_cpy){
		return FAILURE;
	}
	course_cpy->grade = course_org->grade;
	int len = strlen(course_org->course_name) +1;
	course_cpy->course_name = (char*)malloc(sizeof(char)*len);
	if(!(course_cpy->course_name)){
		return FAILURE;
	}
	strcpy(course_cpy->course_name, course_org->course_name);
	*output = course_cpy;
	return SUCCSESS;
}

void destroy_course(void* element) {
	struct course* course_dlt = (struct course*)element;
	free(course_dlt->course_name);
	free(course_dlt);
}


int clone_student(void *element, void **output){
	struct student* student_org = (struct student*)element;
	struct student* student_cpy = 
			(struct student*)malloc(sizeof(struct student));
	if(!student_cpy){
		return FAILURE;
	}
	student_cpy->id = student_org->id;
	int len = strlen(student_org->name) +1;
	student_cpy->name = (char*)malloc(sizeof(char)*len);
	if(!(student_cpy->name)){
		return FAILURE;
	}
	strcpy(student_cpy->name, student_org->name);
	student_cpy->courses = list_init(clone_course, destroy_course);
	if(!(student_cpy->courses)){
		return FAILURE;
	}
	struct iterator* courses_it = list_begin(student_org->courses);
	while(courses_it){
		struct course* course_org = (struct course*)courses_it;
		struct course* course_cpy = 
				(struct course*)malloc(sizeof(struct course));
		void** tmp_out = (void**)&course_cpy;
		clone_course(course_org, tmp_out);
		list_push_back(student_cpy->courses,course_cpy);
		courses_it = list_next(courses_it);
		  
	}
	*output = student_cpy;
	return SUCCSESS;
}

void destroy_student(void *element){
	struct student* student_dlt = (struct student*)element;
	list_destroy(student_dlt->courses);
	free(student_dlt->name);
	free(student_dlt);
	
}

struct grades* grades_init(){
	struct grades* grades;
	grades = (struct grades*)malloc(sizeof(struct grades));
	if(grades==NULL){
		free(grades);
		return NULL;
	}
	grades->students=list_init(clone_student,destroy_student);
	if(!(grades->students)){
		return NULL;
	}
	return grades;
}

void grades_destroy(struct grades* grades){
	list_destroy(grades->students);
	free(grades);
}

int grades_add_student(struct grades *grades, const char *name, int id){
	struct iterator* students_it = list_begin(grades->students);
	int num_of_students = list_size(grades->students);
	if((students_it == NULL && num_of_students != 0) || grades == NULL){
		return FAILURE;
	}
	while(students_it){
		struct student* curr_student = (struct student*)list_get(students_it);
		//if the id is already in list
		if(curr_student->id == id){
			return FAILURE;
		}
		students_it = list_next(students_it);
	}
	struct student* new_student=(struct student*)malloc(sizeof(struct student));
	if(!new_student){
		return FAILURE;
	}
	new_student->id = id;
	int len = strlen(name) +1;
	new_student->name = (char*)malloc(sizeof(char)*len);
	if(new_student->name == NULL){
		free(new_student);
		return FAILURE;
	}
	strcpy(new_student->name, name);
	new_student->courses = list_init(clone_course, destroy_course);
	if(!(new_student->courses)){
		free(new_student->name);
		free(new_student);
		return FAILURE;
	}
	list_push_back(grades->students, new_student);
	list_destroy(new_student->courses);
	free(new_student->name);
	free(new_student);
	return SUCCSESS;
}

int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade){
    //is grade valid
    if (grade>100 || grade<0){
    	return FAILURE;
    }
	struct iterator* students_it = list_begin(grades->students);
	int num_of_students = list_size(grades->students);
	if((students_it == NULL && num_of_students != 0) || grades == NULL){
		return FAILURE;
	}
	//find id
	int curr_id=0;
	while(students_it){
		struct student* curr_student = (struct student*)list_get(students_it);
		if(curr_student->id == id){
			curr_id= curr_student->id;
			break;
		}
		students_it = list_next(students_it);
	}
	struct student* curr_student = (struct student*)list_get(students_it);
	
	// checking if the loop ended without stoping at the given id
	if(curr_id != id){
		return FAILURE;
	}
	
	struct iterator* courses_it = list_begin(curr_student->courses);
	int num_of_courses = list_size(curr_student->courses);
	if(courses_it == NULL && num_of_courses != 0 ){
		return FAILURE;
	}
	while(courses_it){
		struct course* curr_course = (struct course*)list_get(courses_it);
		//checking if the given course already exists

		if(!(strcmp(name, curr_course->course_name))){
			return FAILURE;
		}
		courses_it = list_next(courses_it);		
	}
	struct course* new_course = (struct course*)malloc(sizeof(struct course));
	if(!new_course){
		return FAILURE;
	}
	new_course->grade = grade;
	int len = strlen(name)+1;
	new_course->course_name = (char*)malloc(sizeof(char)*len);
	if(!(new_course->course_name)){
		return FAILURE;
	}
	strcpy(new_course->course_name, name);
	
	list_push_back(curr_student->courses, new_course);
	free(new_course->course_name);
	free(new_course);
	return SUCCSESS;
}

float grades_calc_avg(struct grades *grades, int id, char **out){
	*out = NULL;
	float num_of_courses=0.0;
	float sum = 0.0;
	//find id
	struct iterator* students_it = list_begin(grades->students);
	int num_of_students = list_size(grades->students);
	if((students_it == NULL && num_of_students != 0) || grades == NULL){
		return FAILURE;
	}
	int curr_id=0;
	while(students_it){
		struct student* curr_student = (struct student*)list_get(students_it);
		if(curr_student->id == id){
			curr_id= curr_student->id;
			break;
		}
		students_it = list_next(students_it);
	}
	struct student* curr_student = (struct student*)list_get(students_it);
	// checking if the loop ended without stoping at the given id
	if(curr_id != id){
		return FAILURE;
	}
	
	//sets the variable pointed by "out" to the student's name
	int len = strlen(curr_student->name)+1;
	//The user is responsible for freeing the memory.
	char* out_name = (char*)malloc(sizeof(char)*len);
	if(!out_name){
		return FAILURE;
	} 
	strcpy(out_name, curr_student->name);
	*out = out_name;
	
	//calculating avg
	struct iterator* courses_it = list_begin(curr_student->courses);
	num_of_courses = list_size(curr_student->courses);
	if(courses_it == NULL && num_of_courses != 0 ){
		return FAILURE;
	}
	while(courses_it){
		struct course* curr_course = (struct course*)list_get(courses_it);
		sum += (float)(curr_course->grade);
		courses_it= list_next(courses_it);
	}
	if(num_of_courses == 0){
	  return 0;
	}
	float avg =(float)(sum/num_of_courses);
	return avg;
}

	
int grades_print_student(struct grades *grades, int id){
	//find id
	struct iterator* students_it = list_begin(grades->students);
	int num_of_students = list_size(grades->students);
	if((students_it == NULL && num_of_students != 0) || grades == NULL){
		return FAILURE;
	}
	int curr_id=0;
	while(students_it){
		struct student* curr_student = (struct student*)list_get(students_it);
		if(curr_student->id == id){
			curr_id= curr_student->id;
			break;
		}
		students_it = list_next(students_it);
	}
	struct student* curr_student = (struct student*)list_get(students_it);
	// checking if the loop ended without stoping at the given id
	if(curr_id != id){
		return FAILURE;
	}
	
	//printing courses
	struct iterator* courses_it = list_begin(curr_student->courses);
	int num_of_courses = list_size(curr_student->courses);
	if(courses_it == NULL && num_of_courses != 0 ){
		return FAILURE;
	}
	printf("%s %d:", curr_student->name, curr_student->id);
	while(courses_it){
		struct course* curr_course = (struct course*)list_get(courses_it);
		if(!(list_next(courses_it))){
			printf(" %s %d", curr_course->course_name, curr_course->grade);
			break;
		}
		printf(" %s %d,", curr_course->course_name, curr_course->grade);
		courses_it = list_next(courses_it);
	}
	printf("\n");
	return SUCCSESS;
}


int grades_print_all(struct grades *grades){
	struct iterator* students_it = list_begin(grades->students);
	int num_of_students = list_size(grades->students);
	if((students_it == NULL && num_of_students != 0) || grades == NULL){
		return FAILURE;
	}
	while(students_it){
		struct student* curr_student = (struct student*)list_get(students_it);
		grades_print_student(grades, curr_student->id);
		students_it = list_next(students_it);
	}
	return SUCCSESS;
}
