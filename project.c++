#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <limits>
#include <climits>
#include <cfloat>
#include <map>
#include <ctime>
#include <regex>
#include <set>
#include <queue>
#include <stack>

using namespace std;

// Forward declarations
class Expense;
class ExpenseManager;

// Utility class for input validation, formatting, and utility functions
class Validator {
public:
    // Validates monetary amounts (positive numbers with up to 2 decimal places)
    static bool isValidAmount(const string& str) {
        regex pattern(R"(^\d+(\.\d{1,2})?$)");
        return regex_match(str, pattern) && stod(str) > 0;
    }
    
    // Validates date format and logical date values
    static bool isValidDate(const string& date) {
        regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
        if (!regex_match(date, pattern)) return false;
        
        int year, month, day;
        char dash1, dash2;
        stringstream ss(date);
        ss >> year >> dash1 >> month >> dash2 >> day;
        
        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        
        // Enhanced month-day validation
        vector<int> daysInMonth = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (day > daysInMonth[month - 1]) return false;
        
        // Leap year check for February
        if (month == 2 && day == 29) {
            return isLeapYear(year);
        }
        
        return true;
    }
    
    // Check if year is leap year
    static bool isLeapYear(int year) {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }
    
    // Get current system date in YYYY-MM-DD format
    static string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << (1900 + ltm->tm_year) << "-" 
           << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-" 
           << setfill('0') << setw(2) << ltm->tm_mday;
        return ss.str();
    }
    
    // Remove leading and trailing whitespace
    static string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
    
    // Convert string to uppercase
    static string toUpper(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    // Convert string to lowercase
    static string toLower(const string& str) {
        string result = str;
        transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Format currency display
    static string formatCurrency(double amount) {
        stringstream ss;
        ss << "$" << fixed << setprecision(2) << amount;
        return ss.str();
    }
    
    // Calculate days between two dates
    static int daysBetweenDates(const string& date1, const string& date2) {
        // Simple implementation - can be enhanced with proper date library
        return abs(stoi(date1.substr(8, 2)) - stoi(date2.substr(8, 2)));
    }
};

// Enhanced Expense class with additional features
class Expense {
private:
    static int nextId;          // Auto-incrementing ID generator
    int id;                     // Unique identifier for each expense
    string description;         // Description of the expense
    double amount;              // Amount spent
    string category;            // Category (Food, Transport, etc.)
    string date;                // Date of expense (YYYY-MM-DD)
    string notes;               // Additional notes (NEW)
    bool isRecurring;           // Flag for recurring expenses (NEW)
    string paymentMethod;       // Cash, Card, Online, etc. (NEW)
    string location;            // Where the expense occurred (NEW)
    
public:
    // Default constructor
    Expense() : id(0), amount(0.0), isRecurring(false) {}
    
    // Parameterized constructor for basic expense
    Expense(const string& desc, double amt, const string& cat, const string& dt = "") 
        : id(++nextId), description(desc), amount(amt), category(cat), 
          isRecurring(false), paymentMethod("Cash"), location("") {
        date = dt.empty() ? Validator::getCurrentDate() : dt;
    }
    
    // Full constructor with all fields
    Expense(int expId, const string& desc, double amt, const string& cat, 
            const string& dt, const string& nt = "", bool recurring = false,
            const string& payment = "Cash", const string& loc = "")
        : id(expId), description(desc), amount(amt), category(cat), date(dt),
          notes(nt), isRecurring(recurring), paymentMethod(payment), location(loc) {
        if (id >= nextId) nextId = id + 1;
    }
    
    // Getters - provide read access to private members
    int getId() const { return id; }
    string getDescription() const { return description; }
    double getAmount() const { return amount; }
    string getCategory() const { return category; }
    string getDate() const { return date; }
    string getNotes() const { return notes; }
    bool getIsRecurring() const { return isRecurring; }
    string getPaymentMethod() const { return paymentMethod; }
    string getLocation() const { return location; }
    
    // Setters with validation - provide controlled write access
    void setDescription(const string& desc) {
        if (!desc.empty()) {
            description = Validator::trim(desc);
        }
    }
    
    bool setAmount(double amt) {
        if (amt > 0) {
            amount = amt;
            return true;
        }
        return false;
    }
    
    void setCategory(const string& cat) {
        if (!cat.empty()) {
            category = Validator::trim(cat);
        }
    }
    
    bool setDate(const string& dt) {
        if (Validator::isValidDate(dt)) {
            date = dt;
            return true;
        }
        return false;
    }
    
    void setNotes(const string& nt) { notes = Validator::trim(nt); }
    void setIsRecurring(bool recurring) { isRecurring = recurring; }
    void setPaymentMethod(const string& method) { paymentMethod = Validator::trim(method); }
    void setLocation(const string& loc) { location = Validator::trim(loc); }
    
    // Serialize expense to string for file storage
    string toString() const {
        stringstream ss;
        ss << id << "|" << description << "|" << fixed << setprecision(2) 
           << amount << "|" << category << "|" << date << "|" << notes 
           << "|" << (isRecurring ? "1" : "0") << "|" << paymentMethod 
           << "|" << location;
        return ss.str();
    }
    
    // Deserialize expense from string (file loading)
    static Expense fromString(const string& str) {
        stringstream ss(str);
        string token;
        vector<string> tokens;
        
        while (getline(ss, token, '|')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 9) {
            try {
                int id = stoi(tokens[0]);
                double amount = stod(tokens[2]);
                bool recurring = (tokens[6] == "1");
                return Expense(id, tokens[1], amount, tokens[3], tokens[4], 
                             tokens[5], recurring, tokens[7], tokens[8]);
            } catch (const exception&) {
                return Expense();
            }
        } else if (tokens.size() >= 5) {
            // Backward compatibility with old format
            try {
                int id = stoi(tokens[0]);
                double amount = stod(tokens[2]);
                return Expense(id, tokens[1], amount, tokens[3], tokens[4]);
            } catch (const exception&) {
                return Expense();
            }
        }
        return Expense();
    }
    
    // Display expense in tabular format
    void display() const {
        cout << left << setw(5) << id 
             << setw(20) << description.substr(0, 19)
             << setw(10) << Validator::formatCurrency(amount)
             << setw(12) << category.substr(0, 11)
             << setw(12) << date 
             << setw(8) << paymentMethod.substr(0, 7)
             << setw(3) << (isRecurring ? "Y" : "N") << endl;
    }
    
    // Display detailed expense information
    void displayDetailed() const {
        cout << "\n--- Expense Details ---\n";
        cout << "ID: " << id << endl;
        cout << "Description: " << description << endl;
        cout << "Amount: " << Validator::formatCurrency(amount) << endl;
        cout << "Category: " << category << endl;
        cout << "Date: " << date << endl;
        cout << "Payment Method: " << paymentMethod << endl;
        cout << "Location: " << location << endl;
        cout << "Recurring: " << (isRecurring ? "Yes" : "No") << endl;
        if (!notes.empty()) {
            cout << "Notes: " << notes << endl;
        }
        cout << "----------------------\n";
    }
    
    // Create a copy of the expense (for duplicate feature)
    Expense createCopy() const {
        Expense copy(description + " (Copy)", amount, category, Validator::getCurrentDate());
        copy.setNotes(notes);
        copy.setIsRecurring(isRecurring);
        copy.setPaymentMethod(paymentMethod);
        copy.setLocation(location);
        return copy;
    }
};

int Expense::nextId = 1;

// Enhanced ExpenseManager class with advanced features
class ExpenseManager {
private:
    vector<Expense> expenses;           // Main storage for expenses
    stack<vector<Expense>> undoStack;   // For undo functionality (NEW)
    stack<vector<Expense>> redoStack;   // For redo functionality (NEW)
    string filename;                    // File for data persistence
    set<string> categories;             // Track unique categories (NEW)
    map<string, int> categoryCount;     // Category usage statistics (NEW)
    
    // Enhanced input helper methods
    string getStringInput(const string& prompt, bool allowEmpty = false) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = Validator::trim(input);
            
            if (!input.empty() || allowEmpty) {
                return input;
            }
            cout << "Error: Input cannot be empty. Please try again.\n";
        }
    }
    
    double getAmountInput(const string& prompt) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = Validator::trim(input);
            
            if (Validator::isValidAmount(input)) {
                return stod(input);
            }
            cout << "Error: Please enter a valid positive amount (e.g., 10.50).\n";
        }
    }
    
    string getDateInput(const string& prompt) {
        string input;
        while (true) {
            cout << prompt << " (YYYY-MM-DD) or press Enter for today: ";
            getline(cin, input);
            input = Validator::trim(input);
            
            if (input.empty()) {
                return Validator::getCurrentDate();
            }
            
            if (Validator::isValidDate(input)) {
                return input;
            }
            cout << "Error: Please enter date in YYYY-MM-DD format.\n";
        }
    }
    
    int getIntInput(const string& prompt, int min = 1, int max = INT_MAX) {
        string input;
        while (true) {
            cout << prompt;
            getline(cin, input);
            input = Validator::trim(input);
            
            try {
                int value = stoi(input);
                if (value >= min && value <= max) {
                    return value;
                }
                cout << "Error: Please enter a number between " << min << " and " << max << ".\n";
            } catch (const exception&) {
                cout << "Error: Please enter a valid number.\n";
            }
        }
    }
    
    bool getBoolInput(const string& prompt) {
        string input;
        while (true) {
            cout << prompt << " (y/n): ";
            getline(cin, input);
            input = Validator::toLower(Validator::trim(input));
            
            if (input == "y" || input == "yes" || input == "1") return true;
            if (input == "n" || input == "no" || input == "0") return false;
            
            cout << "Error: Please enter 'y' for yes or 'n' for no.\n";
        }
    }
    
    // Save current state for undo functionality
    void saveState() {
        undoStack.push(expenses);
        // Clear redo stack when new action is performed
        while (!redoStack.empty()) redoStack.pop();
        
        // Limit undo stack size to prevent memory issues
        if (undoStack.size() > 20) {
            stack<vector<Expense>> temp;
            for (int i = 0; i < 19; i++) {
                temp.push(undoStack.top());
                undoStack.pop();
            }
            while (!undoStack.empty()) undoStack.pop();
            while (!temp.empty()) {
                undoStack.push(temp.top());
                temp.pop();
            }
        }
    }
    
    // Update category tracking
    void updateCategoryStats() {
        categories.clear();
        categoryCount.clear();
        
        for (const auto& expense : expenses) {
            categories.insert(expense.getCategory());
            categoryCount[expense.getCategory()]++;
        }
    }
    
    // Display category suggestions based on usage
    void showCategorySuggestions() {
        if (categories.empty()) return;
        
        cout << "Category suggestions: ";
        int count = 0;
        for (const auto& cat : categories) {
            if (count++ < 5) {  // Show top 5 categories
                cout << cat << " ";
            }
        }
        cout << endl;
    }
    
public:
    ExpenseManager(const string& file = "expenses.txt") : filename(file) {
        loadFromFile();
        updateCategoryStats();
    }
    
    ~ExpenseManager() {
        saveToFile();
    }
    
    // Enhanced file operations with error handling
    void saveToFile() {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Warning: Could not save to file " << filename << endl;
            return;
        }
        
        for (const auto& expense : expenses) {
            file << expense.toString() << endl;
        }
        file.close();
    }
    
    void loadFromFile() {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Starting with empty expense list (no existing file found).\n\n";
            return;
        }
        
        string line;
        int loaded = 0, skipped = 0;
        
        while (getline(file, line)) {
            if (!line.empty()) {
                Expense expense = Expense::fromString(line);
                if (expense.getId() > 0) {
                    expenses.push_back(expense);
                    loaded++;
                } else {
                    skipped++;
                }
            }
        }
        file.close();
        
        cout << "\nLoaded " << loaded << " expenses from file";
        if (skipped > 0) {
            cout << " (" << skipped << " corrupted entries skipped)";
        }
        cout << ".\n\n";
    }
    
    // Enhanced add expense with more fields
    void addExpense() {
        cout << "\n=== Add New Expense ===\n";
        
        saveState(); // Save current state for undo
        
        string description = getStringInput("Enter description: ");
        double amount = getAmountInput("Enter amount: $");
        
        showCategorySuggestions();
        string category = getStringInput("Enter category: ");
        
        string date = getDateInput("Enter date");
        string notes = getStringInput("Enter notes (optional): ", true);
        
        cout << "Payment method options: Cash, Card, Online, Check, Transfer\n";
        string paymentMethod = getStringInput("Enter payment method: ");
        
        string location = getStringInput("Enter location (optional): ", true);
        bool isRecurring = getBoolInput("Is this a recurring expense?");
        
        Expense expense(description, amount, category, date);
        expense.setNotes(notes);
        expense.setPaymentMethod(paymentMethod);
        expense.setLocation(location);
        expense.setIsRecurring(isRecurring);
        
        expenses.push_back(expense);
        updateCategoryStats();
        
        cout << "\n* Expense added successfully! ID: " << expense.getId();
        if (isRecurring) {
            cout << " (Marked as recurring)";
        }
        cout << "\n\n";
        
        saveToFile();
    }
    
    // Quick add for frequent expenses
    void quickAddExpense() {
        cout << "\n=== Quick Add Expense ===\n";
        
        saveState();
        
        string description = getStringInput("Description: ");
        double amount = getAmountInput("Amount: $");
        
        // Use most common category as default
        string defaultCategory = "General";
        if (!categoryCount.empty()) {
            auto maxCategory = max_element(categoryCount.begin(), categoryCount.end(),
                [](const pair<string, int>& a, const pair<string, int>& b) {
                    return a.second < b.second;
                });
            defaultCategory = maxCategory->first;
        }
        
        cout << "Category (default: " << defaultCategory << "): ";
        string input;
        getline(cin, input);
        string category = Validator::trim(input);
        if (category.empty()) category = defaultCategory;
        
        Expense expense(description, amount, category);
        expenses.push_back(expense);
        updateCategoryStats();
        
        cout << "* Quick expense added! ID: " << expense.getId() << "\n\n";
        saveToFile();
    }
    
    // Enhanced view with sorting options
    void viewAllExpenses() {
        cout << "\n=== All Expenses ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses found.\n\n";
            return;
        }
        
        cout << "Sort by: 1) Date  2) Amount  3) Category  4) ID (default)\n";
        int sortChoice = getIntInput("Choose sort option (1-4): ", 1, 4);
        
        vector<Expense> sortedExpenses = expenses;
        
        switch (sortChoice) {
            case 1: // Sort by date
                sort(sortedExpenses.begin(), sortedExpenses.end(),
                    [](const Expense& a, const Expense& b) {
                        return a.getDate() > b.getDate(); // Most recent first
                    });
                break;
            case 2: // Sort by amount
                sort(sortedExpenses.begin(), sortedExpenses.end(),
                    [](const Expense& a, const Expense& b) {
                        return a.getAmount() > b.getAmount(); // Highest first
                    });
                break;
            case 3: // Sort by category
                sort(sortedExpenses.begin(), sortedExpenses.end(),
                    [](const Expense& a, const Expense& b) {
                        return a.getCategory() < b.getCategory(); // Alphabetical
                    });
                break;
            default: // Sort by ID (already in order)
                break;
        }
        
        cout << "\n" << left << setw(5) << "ID" 
             << setw(20) << "Description" 
             << setw(10) << "Amount"
             << setw(12) << "Category" 
             << setw(12) << "Date"
             << setw(8) << "Payment"
             << setw(3) << "Rec" << endl;
        cout << string(70, '-') << endl;
        
        for (const auto& expense : sortedExpenses) {
            expense.display();
        }
        
        cout << "\nTotal expenses: " << expenses.size() << endl;
        cout << "Total amount: " << Validator::formatCurrency(getTotalAmount()) << "\n\n";
    }
    
    // NEW: View detailed expense information
    void viewExpenseDetails() {
        cout << "\n=== View Expense Details ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses found.\n\n";
            return;
        }
        
        int id = getIntInput("Enter expense ID to view: ");
        
        auto it = find_if(expenses.begin(), expenses.end(),
            [id](const Expense& e) { return e.getId() == id; });
        
        if (it == expenses.end()) {
            cout << "Expense with ID " << id << " not found.\n\n";
            return;
        }
        
        it->displayDetailed();
    }
    
    // Enhanced category view with statistics
    void viewExpensesByCategory() {
        cout << "\n=== Expenses by Category ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses found.\n\n";
            return;
        }
        
        map<string, vector<Expense>> categoryMap;
        map<string, double> categoryTotals;
        
        for (const auto& expense : expenses) {
            categoryMap[expense.getCategory()].push_back(expense);
            categoryTotals[expense.getCategory()] += expense.getAmount();
        }
        
        double grandTotal = getTotalAmount();
        
        for (const auto& pair : categoryMap) {
            double percentage = (categoryTotals[pair.first] / grandTotal) * 100;
            
            cout << "\n[*] Category: " << pair.first 
                 << " (Total: " << Validator::formatCurrency(categoryTotals[pair.first])
                 << " - " << fixed << setprecision(1) << percentage << "%)\n";
            cout << string(60, '-') << endl;
            
            cout << left << setw(5) << "ID" 
                 << setw(20) << "Description" 
                 << setw(10) << "Amount"
                 << setw(12) << "Date"
                 << setw(8) << "Payment" << endl;
            
            for (const auto& expense : pair.second) {
                cout << left << setw(5) << expense.getId()
                     << setw(20) << expense.getDescription().substr(0, 19)
                     << setw(10) << Validator::formatCurrency(expense.getAmount())
                     << setw(12) << expense.getDate()
                     << setw(8) << expense.getPaymentMethod().substr(0, 7) << endl;
            }
        }
        cout << endl;
    }
    
    // NEW: View recurring expenses
    void viewRecurringExpenses() {
        cout << "\n=== Recurring Expenses ===\n";
        
        vector<Expense> recurringExpenses;
        double totalRecurring = 0;
        
        for (const auto& expense : expenses) {
            if (expense.getIsRecurring()) {
                recurringExpenses.push_back(expense);
                totalRecurring += expense.getAmount();
            }
        }
        
        if (recurringExpenses.empty()) {
            cout << "No recurring expenses found.\n\n";
            return;
        }
        
        cout << left << setw(5) << "ID" 
             << setw(20) << "Description" 
             << setw(10) << "Amount"
             << setw(12) << "Category" 
             << setw(12) << "Date" << endl;
        cout << string(59, '-') << endl;
        
        for (const auto& expense : recurringExpenses) {
            expense.display();
        }
        
        cout << "\nTotal recurring expenses: " << recurringExpenses.size() << endl;
        cout << "Monthly recurring amount: " << Validator::formatCurrency(totalRecurring) << "\n\n";
    }
    
    // Enhanced search with multiple criteria
    void searchExpenses() {
        cout << "\n=== Search Expenses ===\n";
        cout << "1. Search by description\n";
        cout << "2. Search by category\n";
        cout << "3. Search by date range\n";
        cout << "4. Search by amount range\n";
        cout << "5. Search by payment method\n";
        cout << "6. Advanced search (multiple criteria)\n";
        
        int choice = getIntInput("Choose search option (1-6): ", 1, 6);
        
        switch (choice) {
            case 1: searchByDescription(); break;
            case 2: searchByCategory(); break;
            case 3: searchByDateRange(); break;
            case 4: searchByAmountRange(); break;
            case 5: searchByPaymentMethod(); break;
            case 6: advancedSearch(); break;
        }
    }
    
    void searchByDescription() {
        string searchTerm = getStringInput("Enter description to search: ");
        searchTerm = Validator::toLower(searchTerm);
        
        vector<Expense> results;
        for (const auto& expense : expenses) {
            string desc = Validator::toLower(expense.getDescription());
            if (desc.find(searchTerm) != string::npos) {
                results.push_back(expense);
            }
        }
        
        displaySearchResults(results, "Description containing: " + searchTerm);
    }
    
    void searchByCategory() {
        cout << "Available categories: ";
        for (const auto& cat : categories) {
            cout << cat << " ";
        }
        cout << endl;
        
        string category = getStringInput("Enter category to search: ");
        
        vector<Expense> results;
        for (const auto& expense : expenses) {
            if (Validator::toLower(expense.getCategory()) == Validator::toLower(category)) {
                results.push_back(expense);
            }
        }
        
        displaySearchResults(results, "Category: " + category);
    }
    
    void searchByDateRange() {
        string startDate = getDateInput("Enter start date");
        string endDate = getDateInput("Enter end date");
        
        if (startDate > endDate) {
            swap(startDate, endDate);
            cout << "Note: Date range corrected (start < end)\n";
        }
        
        vector<Expense> results;
        for (const auto& expense : expenses) {
            if (expense.getDate() >= startDate && expense.getDate() <= endDate) {
                results.push_back(expense);
            }
        }
        
        displaySearchResults(results, "Date range: " + startDate + " to " + endDate);
    }
    
    // NEW: Search by amount range
    void searchByAmountRange() {
        double minAmount = getAmountInput("Enter minimum amount: $");
        double maxAmount = getAmountInput("Enter maximum amount: $");
        
        if (minAmount > maxAmount) {
            swap(minAmount, maxAmount);
            cout << "Note: Amount range corrected (min < max)\n";
        }
        
        vector<Expense> results;
        for (const auto& expense : expenses) {
            if (expense.getAmount() >= minAmount && expense.getAmount() <= maxAmount) {
                results.push_back(expense);
            }
        }
        
        stringstream criteria;
        criteria << "Amount range: " << Validator::formatCurrency(minAmount) 
                 << " to " << Validator::formatCurrency(maxAmount);
        displaySearchResults(results, criteria.str());
    }
    
    // NEW: Search by payment method
    void searchByPaymentMethod() {
        set<string> paymentMethods;
        for (const auto& expense : expenses) {
            paymentMethods.insert(expense.getPaymentMethod());
        }
        
        cout << "Available payment methods: ";
        for (const auto& method : paymentMethods) {
            cout << method << " ";
        }
        cout << endl;
        
        string paymentMethod = getStringInput("Enter payment method to search: ");
        
        vector<Expense> results;
        for (const auto& expense : expenses) {
            if (Validator::toLower(expense.getPaymentMethod()) == Validator::toLower(paymentMethod)) {
                results.push_back(expense);
            }
        }
        
        displaySearchResults(results, "Payment method: " + paymentMethod);
    }
    
    // NEW: Advanced search with multiple criteria
    void advancedSearch() {
        cout << "\n=== Advanced Search ===\n";
        cout << "Enter search criteria (leave empty to skip):\n";
        
        string description = getStringInput("Description contains: ", true);
        string category = getStringInput("Category: ", true);
        string paymentMethod = getStringInput("Payment method: ", true);
        
        double minAmount = 0, maxAmount = DBL_MAX;
        string amountInput = getStringInput("Minimum amount (or empty): ", true);
        if (!amountInput.empty() && Validator::isValidAmount(amountInput)) {
            minAmount = stod(amountInput);
        }
        
        amountInput = getStringInput("Maximum amount (or empty): ", true);
        if (!amountInput.empty() && Validator::isValidAmount(amountInput)) {
            maxAmount = stod(amountInput);
        }
        
        string startDate = "", endDate = "";
        string dateInput = getStringInput("Start date (YYYY-MM-DD or empty): ", true);
        if (!dateInput.empty() && Validator::isValidDate(dateInput)) {
            startDate = dateInput;
        }
        
        dateInput = getStringInput("End date (YYYY-MM-DD or empty): ", true);
        if (!dateInput.empty() && Validator::isValidDate(dateInput)) {
            endDate = dateInput;
        }
        
        vector<Expense> results;
        for (const auto& expense : expenses) {
            bool matches = true;
            
            // Check description
            if (!description.empty()) {
                if (Validator::toLower(expense.getDescription()).find(Validator::toLower(description)) == string::npos) {
                    matches = false;
                }
            }
            // Check category
            if (!category.empty() && Validator::toLower(expense.getCategory()) != Validator::toLower(category)) {
                matches = false;
            }
            // Check payment method
            if (!paymentMethod.empty() && Validator::toLower(expense.getPaymentMethod()) != Validator::toLower(paymentMethod)) {
                matches = false;
            }
            // Check amount range
            if (expense.getAmount() < minAmount || expense.getAmount() > maxAmount) {
                matches = false;
            }
            // Check date range
            if (!startDate.empty() && expense.getDate() < startDate) {
                matches = false;
            }   
            if (!endDate.empty() && expense.getDate() > endDate) {
                matches = false;
            }
            
            if (matches) {
                results.push_back(expense);
            }
        }
        
        stringstream criteria;
        criteria << "Advanced search with " << 
                    (!description.empty() ? "description, " : "") <<
                    (!category.empty() ? "category, " : "") <<
                    (!paymentMethod.empty() ? "payment method, " : "") <<
                    (minAmount > 0 ? "min amount, " : "") <<
                    (maxAmount < DBL_MAX ? "max amount, " : "") <<
                    (!startDate.empty() ? "start date, " : "") <<
                    (!endDate.empty() ? "end date" : "");
        
        displaySearchResults(results, criteria.str());
    }
    
    // Display search results in a formatted table
    void displaySearchResults(const vector<Expense>& results, const string& criteria) {
        cout << "\n=== Search Results (" << criteria << ") ===\n";
        
        if (results.empty()) {
            cout << "No expenses found matching the criteria.\n\n";
            return;
        }
        
        cout << left << setw(5) << "ID" 
             << setw(20) << "Description" 
             << setw(10) << "Amount"
             << setw(12) << "Category" 
             << setw(12) << "Date"
             << setw(8) << "Payment" << endl;
        cout << string(67, '-') << endl;
        
        double total = 0;
        for (const auto& expense : results) {
            expense.display();
            total += expense.getAmount();
        }
        
        cout << "\nFound " << results.size() << " expenses" << endl;
        cout << "Total amount: " << Validator::formatCurrency(total) << "\n\n";
    }
    
    // Calculate total amount of all expenses
    double getTotalAmount() const {
        double total = 0;
        for (const auto& expense : expenses) {
            total += expense.getAmount();
        }
        return total;
    }
    
    // Enhanced update expense with more options
    void updateExpense() {
        cout << "\n=== Update Expense ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses to update.\n\n";
            return;
        }
        
        saveState(); // Save for undo
        
        int id = getIntInput("Enter expense ID to update: ");
        
        auto it = find_if(expenses.begin(), expenses.end(),
            [id](const Expense& e) { return e.getId() == id; });
        
        if (it == expenses.end()) {
            cout << "Expense with ID " << id << " not found.\n\n";
            return;
        }
        
        cout << "\nCurrent expense details:\n";
        it->displayDetailed();
        
        cout << "\nWhat would you like to update?\n";
        cout << "1. Description\n2. Amount\n3. Category\n4. Date\n";
        cout << "5. Notes\n6. Payment Method\n7. Location\n8. Recurring Status\n9. All fields\n";
        
        int choice = getIntInput("Choose option (1-9): ", 1, 9);
        
        switch (choice) {
            case 1: {
                string newDesc = getStringInput("Enter new description: ");
                it->setDescription(newDesc);
                break;
            }
            case 2: {
                double newAmount = getAmountInput("Enter new amount: $");
                it->setAmount(newAmount);
                break;
            }
            case 3: {
                showCategorySuggestions();
                string newCategory = getStringInput("Enter new category: ");
                it->setCategory(newCategory);
                break;
            }
            case 4: {
                string newDate = getDateInput("Enter new date");
                it->setDate(newDate);
                break;
            }
            case 5: {
                string newNotes = getStringInput("Enter new notes: ", true);
                it->setNotes(newNotes);
                break;
            }
            case 6: {
                string newPayment = getStringInput("Enter new payment method: ");
                it->setPaymentMethod(newPayment);
                break;
            }
            case 7: {
                string newLocation = getStringInput("Enter new location: ", true);
                it->setLocation(newLocation);
                break;
            }
            case 8: {
                bool newRecurring = getBoolInput("Is this a recurring expense?");
                it->setIsRecurring(newRecurring);
                break;
            }
            case 9: {
                string newDesc = getStringInput("Enter new description: ");
                double newAmount = getAmountInput("Enter new amount: $");
                showCategorySuggestions();
                string newCategory = getStringInput("Enter new category: ");
                string newDate = getDateInput("Enter new date");
                string newNotes = getStringInput("Enter new notes: ", true);
                string newPayment = getStringInput("Enter new payment method: ");
                string newLocation = getStringInput("Enter new location: ", true);
                bool newRecurring = getBoolInput("Is this a recurring expense?");
                
                it->setDescription(newDesc);
                it->setAmount(newAmount);
                it->setCategory(newCategory);
                it->setDate(newDate);
                it->setNotes(newNotes);
                it->setPaymentMethod(newPayment);
                it->setLocation(newLocation);
                it->setIsRecurring(newRecurring);
                break;
            }
        }
        
        updateCategoryStats();
        cout << "\n* Expense updated successfully!\n\n";
        saveToFile();
    }
    
    // Enhanced delete with confirmation
    void deleteExpense() {
        cout << "\n=== Delete Expense ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses to delete.\n\n";
            return;
        }
        
        saveState(); // Save for undo
        
        int id = getIntInput("Enter expense ID to delete: ");
        
        auto it = find_if(expenses.begin(), expenses.end(),
            [id](const Expense& e) { return e.getId() == id; });
        
        if (it == expenses.end()) {
            cout << "Expense with ID " << id << " not found.\n\n";
            return;
        }
        
        cout << "\nExpense to be deleted:\n";
        it->displayDetailed();
        
        bool confirm = getBoolInput("\nAre you sure you want to delete this expense?");
        
        if (confirm) {
            expenses.erase(it);
            updateCategoryStats();
            cout << "* Expense deleted successfully!\n\n";
            saveToFile();
        } else {
            cout << "Delete operation cancelled.\n\n";
        }
    }
    
    // NEW: Duplicate expense
    void duplicateExpense() {
        cout << "\n=== Duplicate Expense ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses to duplicate.\n\n";
            return;
        }
        
        saveState();
        
        int id = getIntInput("Enter expense ID to duplicate: ");
        
        auto it = find_if(expenses.begin(), expenses.end(),
            [id](const Expense& e) { return e.getId() == id; });
        
        if (it == expenses.end()) {
            cout << "Expense with ID " << id << " not found.\n\n";
            return;
        }
        
        Expense duplicate = it->createCopy();
        expenses.push_back(duplicate);
        updateCategoryStats();
        
        cout << "* Expense duplicated successfully! New ID: " << duplicate.getId() << "\n\n";
        saveToFile();
    }
    
    // NEW: Undo last operation
    void undoLastOperation() {
        if (undoStack.empty()) {
            cout << "No operations to undo.\n\n";
            return;
        }
        
        redoStack.push(expenses);
        expenses = undoStack.top();
        undoStack.pop();
        updateCategoryStats();
        saveToFile();
        
        cout << "* Last operation undone successfully!\n\n";
    }
    
    // NEW: Redo last undone operation
    void redoLastOperation() {
        if (redoStack.empty()) {
            cout << "No operations to redo.\n\n";
            return;
        }
        
        undoStack.push(expenses);
        expenses = redoStack.top();
        redoStack.pop();
        updateCategoryStats();
        saveToFile();
        
        cout << "* Last operation redone successfully!\n\n";
    }
    
    // Enhanced summary with detailed analytics
    void generateSummary() {
        cout << "\n=== Expense Summary & Analytics ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses found.\n\n";
            return;
        }
        
        double total = getTotalAmount();
        cout << "[*] Overall Statistics:\n";
        cout << "Total expenses: " << expenses.size() << endl;
        cout << "Total amount: " << Validator::formatCurrency(total) << endl;
        cout << "Average expense: " << Validator::formatCurrency(total / expenses.size()) << endl;
        
        // Find highest and lowest expenses
        auto maxExpense = max_element(expenses.begin(), expenses.end(),
            [](const Expense& a, const Expense& b) { return a.getAmount() < b.getAmount(); });
        auto minExpense = min_element(expenses.begin(), expenses.end(),
            [](const Expense& a, const Expense& b) { return a.getAmount() < b.getAmount(); });
        
        cout << "Highest expense: " << Validator::formatCurrency(maxExpense->getAmount()) 
             << " (" << maxExpense->getDescription() << ")\n";
        cout << "Lowest expense: " << Validator::formatCurrency(minExpense->getAmount()) 
             << " (" << minExpense->getDescription() << ")\n";
        
        // Category breakdown
        map<string, double> categoryTotals;
        map<string, int> categoryCount;
        for (const auto& expense : expenses) {
            categoryTotals[expense.getCategory()] += expense.getAmount();
            categoryCount[expense.getCategory()]++;
        }
        
        cout << "\n[*] Category Breakdown:\n";
        cout << left << setw(15) << "Category" << setw(10) << "Count" 
             << setw(12) << "Total" << setw(10) << "Avg" << "Percentage" << endl;
        cout << string(65, '-') << endl;
        
        for (const auto& pair : categoryTotals) {
            double percentage = (pair.second / total) * 100;
            double average = pair.second / categoryCount[pair.first];
            cout << left << setw(15) << pair.first.substr(0, 14)
                 << setw(10) << categoryCount[pair.first]
                 << setw(12) << Validator::formatCurrency(pair.second).substr(0, 11)
                 << setw(10) << Validator::formatCurrency(average).substr(0, 9)
                 << fixed << setprecision(1) << percentage << "%" << endl;
        }
        
        // Payment method breakdown
        map<string, double> paymentTotals;
        for (const auto& expense : expenses) {
            paymentTotals[expense.getPaymentMethod()] += expense.getAmount();
        }
        
        cout << "\n[*] Payment Method Breakdown:\n";
        for (const auto& pair : paymentTotals) {
            double percentage = (pair.second / total) * 100;
            cout << left << setw(15) << pair.first << ": " 
                 << Validator::formatCurrency(pair.second) 
                 << " (" << fixed << setprecision(1) << percentage << "%)" << endl;
        }
        
        // Monthly breakdown
        map<string, double> monthlyTotals;
        for (const auto& expense : expenses) {
            string month = expense.getDate().substr(0, 7); // YYYY-MM
            monthlyTotals[month] += expense.getAmount();
        }
        
        if (monthlyTotals.size() > 1) {
            cout << "\n[*] Monthly Breakdown:\n";
            for (const auto& pair : monthlyTotals) {
                cout << pair.first << ": " << Validator::formatCurrency(pair.second) << endl;
            }
        }
        
        // Recurring expenses summary
        double recurringTotal = 0;
        int recurringCount = 0;
        for (const auto& expense : expenses) {
            if (expense.getIsRecurring()) {
                recurringTotal += expense.getAmount();
                recurringCount++;
            }
        }
        
        if (recurringCount > 0) {
            cout << "\n[*] Recurring Expenses:\n";
            cout << "Count: " << recurringCount << endl;
            cout << "Monthly total: " << Validator::formatCurrency(recurringTotal) << endl;
            cout << "Annual projection: " << Validator::formatCurrency(recurringTotal * 12) << endl;
        }
        
        cout << endl;
    }
    
    // Export expenses to CSV
    void exportToCSV() {
        cout << "\n=== Export to CSV ===\n";
        
        if (expenses.empty()) {
            cout << "No expenses to export.\n\n";
            return;
        }
        
        string csvFilename = getStringInput("Enter CSV filename (without .csv extension): ");
        csvFilename += ".csv";
        
        ofstream csvFile(csvFilename);
        if (!csvFile.is_open()) {
            cout << "Error: Could not create CSV file.\n\n";
            return;
        }
        
        // Write header
        csvFile << "ID,Description,Amount,Category,Date,Notes,Recurring,PaymentMethod,Location\n";
        
        // Write data
        for (const auto& expense : expenses) {
            csvFile << expense.getId() << ","
                    << "\"" << expense.getDescription() << "\","
                    << fixed << setprecision(2) << expense.getAmount() << ","
                    << "\"" << expense.getCategory() << "\","
                    << expense.getDate() << ","
                    << "\"" << expense.getNotes() << "\","
                    << (expense.getIsRecurring() ? "Yes" : "No") << ","
                    << "\"" << expense.getPaymentMethod() << "\","
                    << "\"" << expense.getLocation() << "\"\n";
        }
        
        csvFile.close();
        cout << "* Expenses exported to " << csvFilename << " successfully!\n\n";
    }
    
    // Backup and restore
    void backupData() {
        string backupFile = filename + ".backup." + to_string(time(0));
        ofstream backup(backupFile);
        
        if (!backup.is_open()) {
            cout << "Error: Could not create backup file.\n\n";
            return;
        }
        
        for (const auto& expense : expenses) {
            backup << expense.toString() << endl;
        }
        backup.close();
        
        cout << "* Data backed up to: " << backupFile << "\n\n";
    }
    
    void clearAllData() {
        cout << "\n=== Clear All Data ===\n";
        cout << "WARNING: This will permanently delete ALL expenses!\n";
        
        string confirmation = getStringInput("Type 'DELETE ALL' to confirm: ");
        
        if (confirmation == "DELETE ALL") {
            saveState(); // Save for undo
            expenses.clear();
            updateCategoryStats();
            saveToFile();
            cout << "* All expenses have been deleted.\n\n";
        } else {
            cout << "Operation cancelled.\n\n";
        }
    }
};

// Enhanced main application class with comprehensive menu system
class ExpenseTrackerApp {
private:
    ExpenseManager manager;
    bool running;
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void pauseScreen() {
        cout << "Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string dummy;
        getline(cin, dummy);
    }
    
    void displayMainMenu() {
        cout << "\n========================================\n";
        cout << "        ENHANCED EXPENSE TRACKER        \n";
        cout << "========================================\n";
        cout << "  EXPENSE MANAGEMENT                    \n";
        cout << "  1.  Add Expense                       \n";
        cout << "  2.  Quick Add Expense                 \n";
        cout << "  3.  View All Expenses                 \n";
        cout << "  4.  View Expense Details              \n";
        cout << "  5.  View Expenses by Category         \n";
        cout << "  6.  View Recurring Expenses           \n";
        cout << "                                        \n";
        cout << "  SEARCH & FILTER                       \n";
        cout << "  7.  Search Expenses                   \n";
        cout << "                                        \n";
        cout << "  EDIT & MANAGE                         \n";
        cout << "  8.  Update Expense                    \n";
        cout << "  9.  Delete Expense                    \n";
        cout << "  10. Duplicate Expense                 \n";
        cout << "                                        \n";
        cout << "  UNDO/REDO                             \n";
        cout << "  11. Undo Last Operation               \n";
        cout << "  12. Redo Last Operation               \n";
        cout << "                                        \n";
        cout << "  REPORTS & ANALYTICS                   \n";
        cout << "  13. Generate Summary & Analytics      \n";
        cout << "  14. Export to CSV                     \n";
        cout << "                                        \n";
        cout << "  UTILITIES                             \n";
        cout << "  15. Backup Data                       \n";
        cout << "  16. Clear All Data                    \n";
        cout << "                                        \n";
        cout << "  0.  Exit Application                  \n";
        cout << "========================================\n";
    }
    
    int getMenuChoice() {
        string input;
        while (true) {
            cout << "\nEnter your choice (0-16): ";
            getline(cin, input);
            
            try {
                int choice = stoi(input);
                if (choice >= 0 && choice <= 16) {
                    return choice;
                }
                cout << "Error: Please enter a number between 0 and 16.\n";
            } catch (const exception&) {
                cout << "Error: Please enter a valid number.\n";
            }
        }
    }
    
public:
    ExpenseTrackerApp() : running(true) {
        cout << "========================================\n";
        cout << "     Welcome to Enhanced Expense       \n";
        cout << "           Tracker v2.0!               \n";
        cout << "                                       \n";
        cout << "    Your comprehensive solution for    \n";
        cout << "      managing personal finances       \n";
        cout << "========================================\n\n";
    }
    
    void run() {
        while (running) {
            displayMainMenu();
            int choice = getMenuChoice();
            
            // Clear screen for better user experience
            cout << "\n" << string(50, '=') << "\n";
            
            switch (choice) {
                case 1:
                    manager.addExpense();
                    pauseScreen();
                    break;
                case 2:
                    manager.quickAddExpense();
                    pauseScreen();
                    break;
                case 3:
                    manager.viewAllExpenses();
                    pauseScreen();
                    break;
                case 4:
                    manager.viewExpenseDetails();
                    pauseScreen();
                    break;
                case 5:
                    manager.viewExpensesByCategory();
                    pauseScreen();
                    break;
                case 6:
                    manager.viewRecurringExpenses();
                    pauseScreen();
                    break;
                case 7:
                    manager.searchExpenses();
                    pauseScreen();
                    break;
                case 8:
                    manager.updateExpense();
                    pauseScreen();
                    break;
                case 9:
                    manager.deleteExpense();
                    pauseScreen();
                    break;
                case 10:
                    manager.duplicateExpense();
                    pauseScreen();
                    break;
                case 11:
                    manager.undoLastOperation();
                    pauseScreen();
                    break;
                case 12:
                    manager.redoLastOperation();
                    pauseScreen();
                    break;
                case 13:
                    manager.generateSummary();
                    pauseScreen();
                    break;
                case 14:
                    manager.exportToCSV();
                    pauseScreen();
                    break;
                case 15:
                    manager.backupData();
                    pauseScreen();
                    break;
                case 16:
                    manager.clearAllData();
                    pauseScreen();
                    break;
                case 0:
                    cout << "\n========================================\n";
                    cout << "     Thank you for using Enhanced      \n";
                    cout << "          Expense Tracker!             \n";
                    cout << "                                       \n";
                    cout << "    Your data has been saved           \n";
                    cout << "         automatically.                \n";
                    cout << "                                       \n";
                    cout << "        Have a great day!              \n";
                    cout << "========================================\n";
                    running = false;
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n\n";
                    pauseScreen();
            }
        }
    }
};

// Main function with error handling
int main() {
    try {
        ExpenseTrackerApp app;
        app.run();
    } catch (const exception& e) {
        cout << "\nFatal error: " << e.what() << endl;
        cout << "The application will now exit.\n";
        return 1;
    } catch (...) {
        cout << "\nAn unexpected error occurred.\n";
        cout << "The application will now exit.\n";
        return 1;
    }
    
    return 0;
}