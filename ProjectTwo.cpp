/*
 * ProjectTwo.cpp
 * Author: Johana Morris
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>

using namespace std;

// Structure to store information about each course
struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

// Hash table to store courses using their course numbers
unordered_map<string, Course> courses;
bool dataLoaded = false;

// Remove extra spaces from the beginning and end of a field
string trim(string text) {
    size_t first = text.find_first_not_of(" \t\r\n");

    if (first == string::npos) {
        return "";
    }

    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Convert course numbers to uppercase so searches work with lowercase input
string toUpperCase(string text) {
    for (char& character : text) {
        character = static_cast<char>(
            toupper(static_cast<unsigned char>(character))
        );
    }

    return text;
}

// Read the CSV file and load valid courses into the hash table
bool loadCourses(const string& fileName) {

    // Clear previous data before loading a new file
    courses.clear();
    dataLoaded = false;

    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file "
             << fileName << "." << endl;
        return false;
    }

    // Store courses separately until the file has been checked
    unordered_map<string, Course> newCourses;
    string line;
    int lineNumber = 0;

    while (getline(inputFile, line)) {
        ++lineNumber;

        if (trim(line).empty()) {
            continue;
        }

        // Separate each line into individual fields
        stringstream lineStream(line);
        string field;
        vector<string> fields;

        while (getline(lineStream, field, ',')) {
            fields.push_back(trim(field));
        }

        // Keep track of an empty field if the line ends with a comma
        if (!line.empty() && line.back() == ',') {
            fields.push_back("");
        }

        // Check for missing course numbers or titles
        if (fields.size() < 2 ||
            fields[0].empty() ||
            fields[1].empty()) {

            cout << "Error: Missing course number or title on line "
                 << lineNumber << "." << endl;
            return false;
        }

        // Create a course object using the file information
        Course course;
        course.courseNumber = toUpperCase(fields[0]);
        course.courseTitle = fields[1];

        // Check for duplicate course numbers
        if (newCourses.find(course.courseNumber) != newCourses.end()) {
            cout << "Error: Duplicate course number "
                 << course.courseNumber
                 << " on line " << lineNumber << "." << endl;
            return false;
        }

        // Store any prerequisites associated with the course
        for (size_t i = 2; i < fields.size(); ++i) {

            // Skip empty fields because some courses have no prerequisites
            if (fields[i].empty()) {
                continue;
            }

            course.prerequisites.push_back(
                toUpperCase(fields[i])
            );
        }
        // Add the course to the temporary hash table
        newCourses[course.courseNumber] = course;
    }

    inputFile.close();

    // Check whether the file contained any courses
    if (newCourses.empty()) {
        cout << "Error: The file does not contain any courses."
             << endl;
        return false;
    }

    // Check that all prerequisite courses exist
    for (const auto& entry : newCourses) {

        for (const string& prerequisite :
             entry.second.prerequisites) {

            if (newCourses.find(prerequisite) == newCourses.end()) {

                cout << "Error: Prerequisite "
                     << prerequisite << " for "
                     << entry.first
                     << " was not found." << endl;
                return false;
            }
        }
    }

    // Add the checked courses to the main hash table
    courses = newCourses;
    dataLoaded = true;

    cout << "Courses loaded successfully!" << endl;
    return true;
}

// Print all courses in alphanumeric order
void printCourseList() {

    // Prevent printing before the course data is loaded
    if (!dataLoaded) {
        cout << "Please load the course data first." << endl;
        return;
    }

    // Copy course numbers from the hash table into a vector
    vector<string> courseNumbers;

    for (const auto& entry : courses) {
        courseNumbers.push_back(entry.first);
    }

    // Sort course numbers from lowest to highest
    sort(courseNumbers.begin(), courseNumbers.end());

    cout << "Here is a sample schedule:" << endl;

    // Print each course in sorted order
    for (const string& number : courseNumbers) {

        const Course& course = courses.at(number);

        cout << course.courseNumber << ", "
             << course.courseTitle << endl;
    }
}

// Search for a course and display its information
void printCourse() {

    // Prevent searching before the course data is loaded
    if (!dataLoaded) {
        cout << "Please load the course data first." << endl;
        return;
    }

    string courseNumber;

    cout << "What course do you want to know about? ";
    getline(cin, courseNumber);

    // Convert input to uppercase before searching
    courseNumber = toUpperCase(trim(courseNumber));

    auto found = courses.find(courseNumber);

    // Display an error if the course does not exist
    if (found == courses.end()) {
        cout << "Course not found." << endl;
        return;
    }

    const Course& course = found->second;

    cout << course.courseNumber << ", "
         << course.courseTitle << endl;

    cout << "Prerequisites: ";

    // Display None if the course has no prerequisites
    if (course.prerequisites.empty()) {
        cout << "None";
    }
    else {

        // Display prerequisite course numbers and titles
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {

            const string& number = course.prerequisites[i];
            const Course& prerequisite = courses.at(number);

            if (i > 0) {
                cout << ", ";
            }

            cout << number << " ("
                 << prerequisite.courseTitle << ")";
        }
    }

    cout << endl;
}

// Display the menu until the user chooses to exit
int main() {

    string choice;

    cout << "Welcome to the course planner." << endl;

    while (true) {

        // Display the available menu options
        cout << "\n1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;

        cout << "What would you like to do? ";

        if (!getline(cin, choice)) {
            break;
        }

        choice = trim(choice);

        // Load course information from the selected file
        if (choice == "1") {

            string fileName;

            cout << "Enter the file name: ";

            if (!getline(cin, fileName)) {
                break;
            }

            loadCourses(trim(fileName));
        }

        // Print the complete sorted course list
        else if (choice == "2") {
            printCourseList();
        }

        // Search for an individual course
        else if (choice == "3") {
            printCourse();
        }

        // Exit the program
        else if (choice == "9") {
            cout << "Thank you for using the course planner!"
                 << endl;
            break;
        }

        // Display an error for invalid menu selections
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    return 0;
}
