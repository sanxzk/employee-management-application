#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <sstream>

// Structure to represent clock-in and clock-out events
struct AttendanceRecord {
    time_t timestamp;
    bool isClockIn;

    AttendanceRecord(time_t timestamp, bool isClockIn)
        : timestamp(timestamp), isClockIn(isClockIn) {}
};

// Employee structure to store employee details
struct Employee {
    int id;
    std::string name;
    std::string designation;
    std::string department;
    std::vector<AttendanceRecord> attendanceRecords;

    Employee(int id, const std::string& name, const std::string& designation, const std::string& department)
        : id(id), name(name), designation(designation), department(department) {}

    // Function to add a clock-in record
    void addClockIn(time_t timestamp) {
        attendanceRecords.emplace_back(timestamp, true);
    }

    // Function to add a clock-out record
    void addClockOut(time_t timestamp) {
        attendanceRecords.emplace_back(timestamp, false);
    }

    // Function to check if the employee is currently clocked in
    bool isClockedIn() const {
        int countClockIns = 0;
        int countClockOuts = 0;
        for (const auto& record : attendanceRecords) {
            if (record.isClockIn) {
                ++countClockIns;
            } else {
                ++countClockOuts;
            }
        }
        return countClockIns > countClockOuts;
    }
};

// Function to save employee data to a file
void saveData(const std::vector<Employee>& employees) {
    std::ofstream outFile("employees.txt");
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return;
    }

    for (const auto& emp : employees) {
        outFile << emp.id << " " << emp.name << " " << emp.designation << " " << emp.department << std::endl;
        for (const auto& record : emp.attendanceRecords) {
            outFile << (record.isClockIn ? "IN " : "OUT ") << record.timestamp << std::endl;
        }
        outFile << "END\n";
    }

    outFile.close();
}

// Function to load employee data from a file
void loadData(std::vector<Employee>& employees) {
    std::ifstream inFile("employees.txt");
    if (!inFile) {
        std::cerr << "No previous data found. Creating a new file." << std::endl;
        std::ofstream outFile("employees.txt");
        if (!outFile) {
            std::cerr << "Error: Unable to create file for writing." << std::endl;
            return;
        }
        outFile.close();
        return;
    }

    employees.clear();
    std::string line;
    while (getline(inFile, line)) {
        std::stringstream ss(line);
        int id;
        std::string name, designation, department;
        ss >> id >> name >> designation >> department;
        employees.emplace_back(id, name, designation, department);

        while (getline(inFile, line) && line != "END") {
            std::stringstream ss2(line);
            std::string type;
            time_t timestamp;
            ss2 >> type >> timestamp;
            employees.back().attendanceRecords.emplace_back(timestamp, type == "IN");
        }
    }

    inFile.close();
}

// Function to register a new user
void registerUser(std::vector<Employee>& employees) {
    std::string name, designation, department;
    std::cout << "Enter name: ";
    std::cin >> name;
    std::cout << "Enter designation: ";
    std::cin >> designation;
    std::cout << "Enter department: ";
    std::cin >> department;
    employees.emplace_back(employees.size() + 1, name, designation, department);
    std::cout << "New user registered with ID: " << employees.size() << std::endl;

    // Save data to file after registering a new user
    saveData(employees);
}

// Function to delete a user
void deleteUser(std::vector<Employee>& employees) {
    int id;
    std::cout << "Enter ID of the user to delete: ";
    std::cin >> id;
    auto it = std::find_if(employees.begin(), employees.end(), [id](const Employee& emp)
                      { return emp.id == id; });
    if (it != employees.end()) {
        employees.erase(it);
        std::cout << "User with ID " << id << " deleted successfully." << std::endl;

        // Save data to file after deleting a user
        saveData(employees);
    } else {
        std::cout << "User not found." << std::endl;
    }
}

// Function to clock in for an employee
void clockIn(Employee& emp, std::vector<Employee>& employees) {
    emp.addClockIn(time(nullptr));
    std::cout << "Clocked in successfully for employee " << emp.name << std::endl;

    // Save data to file after clocking in
    saveData(employees);
}

// Function to clock out for an employee
void clockOut(Employee& emp, std::vector<Employee>& employees) {
    emp.addClockOut(time(nullptr));
    std::cout << "Clocked out successfully for employee " << emp.name << std::endl;

    // Save data to file after clocking out
    saveData(employees);
}

// Function to generate attendance report for a specific employee
void generateEmployeeReport(const Employee& emp) {
    std::cout << "Attendance Report for Employee ID: " << emp.id << std::endl;
    std::cout << "Name: " << emp.name << std::endl;
    std::cout << "Designation: " << emp.designation << std::endl;
    std::cout << "Department: " << emp.department << std::endl << std::endl;

    std::cout << std::setw(12) << "Date" << std::setw(10) << "Day" << std::setw(15) << "Clock In Time" << std::setw(15) << "Clock Out Time" << std::setw(15) << "Working Hours" << std::setw(20) << "Remark" << std::endl;
    std::cout << std::setfill('-') << std::setw(76) << std::endl;
    std::cout << std::setfill(' ');

    for (size_t i = 0; i < emp.attendanceRecords.size(); ++i) {
        const auto& record = emp.attendanceRecords[i];
        if (record.isClockIn) {
            std::cout << "IN ";
        } else {
            std::cout << "OUT";
        }

        struct tm* timeInfo = localtime(&record.timestamp);
        char dateStr[12], dayStr[10], timeStr[9];
        strftime(dateStr, sizeof(dateStr), "%d-%m-%Y", timeInfo);
        strftime(dayStr, sizeof(dayStr), "%A", timeInfo);
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", timeInfo);
        std::cout << std::setw(12) << dateStr << std::setw(10) << dayStr << std::setw(15) << timeStr;

        if (!record.isClockIn) {
            double workingHours = difftime(record.timestamp, emp.attendanceRecords[i - 1].timestamp) / 3600.0;
            std::string remark;
            if (workingHours < 4.0) {
                remark = "Short Attendance";
            } else if (workingHours >= 4.0 && workingHours < 8.0) {
                remark = "Half Day";
            } else {
                remark = "Full Day";
            }
            std::cout << std::setw(15) << std::fixed << std::setprecision(2) << workingHours << std::setw(20) << remark << std::endl;
        } else {
            std::cout << std::endl;
        }
    }
}

// Function to list all users with their details and last clock-in/clock-out time
void listAllUsers(const std::vector<Employee>& employees) {
    std::cout << "List of All Users:\n";
    std::cout << std::setw(5) << "ID" << std::setw(15) << "Name" << std::setw(20) << "Designation" << std::setw(20) << "Department" << std::setw(20) << "Last Clock In" << std::setw(20) << "Last Clock Out" << std::endl;
    std::cout << std::setfill('-') << std::setw(100) << std::endl;
    std::cout << std::setfill(' ');
    for (const auto& emp : employees) {
        time_t lastClockIn = 0;
        time_t lastClockOut = 0;
        for (const auto& record : emp.attendanceRecords) {
            if (record.isClockIn) {
                lastClockIn = record.timestamp;
            } else {
                lastClockOut = record.timestamp;
            }
        }
        struct tm* timeInfoIn = localtime(&lastClockIn);
        struct tm* timeInfoOut = localtime(&lastClockOut);
        char timeStrIn[9], timeStrOut[9];
        strftime(timeStrIn, sizeof(timeStrIn), "%H:%M:%S", timeInfoIn);
        strftime(timeStrOut, sizeof(timeStrOut), "%H:%M:%S", timeInfoOut);
        std::cout << std::setw(5) << emp.id << std::setw(15) << emp.name << std::setw(20) << emp.designation << std::setw(20) << emp.department;
        if (lastClockIn != 0) {
            std::cout << std::setw(20) << timeStrIn;
        } else {
            std::cout << std::setw(20) << "N/A";
        }
        if (lastClockOut != 0) {
            std::cout << std::setw(20) << timeStrOut << std::endl;
        } else {
            std::cout << std::setw(20) << "N/A" << std::endl;
        }
    }
}

int main() {
    std::vector<Employee> employees;
    loadData(employees); // Load data from file

    int choice;
    do {
        std::cout << "\nEmployee Attendance Management System\n";
        std::cout << "1. Register New User\n";
        std::cout << "2. Delete User\n";
        std::cout << "3. Clock In\n";
        std::cout << "4. Clock Out\n";
        std::cout << "5. Generate Attendance Report\n";
        std::cout << "6. List All Users\n";
        std::cout << "7. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                registerUser(employees);
                break;
            case 2:
                deleteUser(employees);
                break;
            case 3: {
                int id;
                std::cout << "Enter employee ID to clock in: ";
                std::cin >> id;
                auto it = std::find_if(employees.begin(), employees.end(), [id](const Employee& emp)
                                  { return emp.id == id; });
                if (it != employees.end()) {
                    clockIn(*it, employees); // Pass employees vector as an argument
                } else {
                    std::cout << "Employee not found." << std::endl;
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "Enter employee ID to clock out: ";
                std::cin >> id;
                auto it = std::find_if(employees.begin(), employees.end(), [id](const Employee& emp)
                                  { return emp.id == id; });
                if (it != employees.end()) {
                    clockOut(*it, employees); // Pass employees vector as an argument
                } else {
                    std::cout << "Employee not found." << std::endl;
                }
                break;
            }
            case 5: {
                int id;
                std::cout << "Enter employee ID to generate report: ";
                std::cin >> id;
                auto it = std::find_if(employees.begin(), employees.end(), [id](const Employee& emp)
                                  { return emp.id == id; });
                if (it != employees.end()) {
                    generateEmployeeReport(*it);
                } else {
                    std::cout << "Employee not found." << std::endl;
                }
                break;
            }
            case 6:
                listAllUsers(employees);
                break;
            case 7:
                std::cout << "Saving data and exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 7);

    return 0;
}
