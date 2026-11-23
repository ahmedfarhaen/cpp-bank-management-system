#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
using namespace std;

struct stClient {
    string Account_Number;
    string PIN_Code;
    string Client_Name;
    string Client_Phone;
    double Account_Balance = 0;
    bool MarkToDelete = false;
};

struct stUsers {
    string UserName;
    string User_Password;
    int Permissions = 0;
    bool MarkToDelete = false;
};


const string ClientsFileName = "Client.txt";
const string UsersFileName = "Users.txt";

stUsers CurrentUser;


enum enMenuOption {
    Login = 0, Show_Client_List = 1, Add_New_Client = 2, Delete_Client = 3,
    Update_ClientInfo = 4, Find_Client = 5, Transactions = 6, Manage_User = 7, LogOut = 8
};

enum enTransactions { eDeposit = 1, eWithdraw = 2, eTotalBalances = 3, eMainMenu = 4 };

enum enManageUser {eListUser = 1, eAdd_New_User = 2, eDelete_User = 3, eUpdate_User = 4, eFind_User = 5, eMain_Menue = 6};


enum enPermissions
{
    All = -1, eShow_Client_List = 1, eAdding_New_Client = 2,
    eDeletet_Client = 4, eUpdate_Client = 8, eFind_Client = 16,
    eTransactions = 32, eManage_Users = 64
};


// ==========================================
// Helper & File I/O Functions
// ==========================================

vector<string> SplitLine(string Line, string Separator = "#//#")
{
    vector<string> vClient;
    rsize_t Pos = 0;
    string sWord;
    while ((Pos = Line.find(Separator)) != string::npos)
    {
        sWord = Line.substr(0, Pos);
        if (sWord != "")
        {
            vClient.push_back(sWord);
        }
        Line.erase(0, Pos + Separator.length());
    }
    if (Line != "")
    {
        vClient.push_back(Line);
    }
    return vClient;
}

stClient ConvertLineToRecord(const string& Line)
{
    vector<string> vClient = SplitLine(Line);
    stClient Client;

    if (vClient.size() >= 5)
    {
        Client.Account_Number = vClient[0];
        Client.PIN_Code = vClient[1];
        Client.Client_Name = vClient[2];
        Client.Client_Phone = vClient[3];
        Client.Account_Balance = stod(vClient[4]);
    }
    return Client;
}

string ConvertRecordToLine(const stClient& Client, string Separator = "#//#")
{
    string sClient = "";
    sClient += Client.Account_Number + Separator;
    sClient += Client.PIN_Code + Separator;
    sClient += Client.Client_Name + Separator;
    sClient += Client.Client_Phone + Separator;
    sClient += to_string(Client.Account_Balance);
    return sClient;
}

vector<stClient> LoadClientsFromFile(const string& FileName)
{
    vector<stClient> vClients;
    fstream MyFile;
    MyFile.open(FileName, ios::in);

    if (MyFile.is_open())
    {
        string Line;
        while (getline(MyFile, Line))
        {
            if (Line != "")
            {
                vClients.push_back(ConvertLineToRecord(Line));
            }
        }
        MyFile.close();
    }
    return vClients;
}

void SaveClientsRecordInFile(const string& FileName, const vector<stClient>& vClients)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out);
    if (MyFile.is_open())
    {
        for (const stClient& C : vClients)
        {
            if (!C.MarkToDelete)
            {
                MyFile << ConvertRecordToLine(C) << "\n";
            }
        }
        MyFile.close();
    }
}

void AddDataLineToFile(const string& FileName, const string& Line)
{
    fstream MyFile;
    MyFile.open(FileName, ios::out | ios::app);
    if (MyFile.is_open())
    {
        MyFile << Line << "\n";
        MyFile.close();
    }
}

bool ClientExistsByAccountNumber(const string& FileName, const string& AccountNumber)
{
    fstream MyFile;
    MyFile.open(FileName, ios::in);
    if (MyFile.is_open())
    {
        string Line;
        while (getline(MyFile, Line))
        {
            stClient Client = ConvertLineToRecord(Line);
            if (Client.Account_Number == AccountNumber)
            {
                MyFile.close();
                return true;
            }
        }
        MyFile.close();
    }
    return false;
}

bool FindClientRecordByAccountNumber(const string& AccountNumber, const vector<stClient>& vClients, stClient& Client)
{
    for (const stClient& C : vClients)
    {
        if (C.Account_Number == AccountNumber)
        {
            Client = C;
            return true;
        }
    }
    return false;
}

// ==========================================
// UI & Navigation Helpers
// ==========================================

void ShowManageUserScreen();

void ShowTransactionsScreen();

void ShowMainMenu();

bool IsUserHasPermissions(enPermissions PermissionsRequired);



void GoBackToMainMenu()
{
    cout << "\n\nPress any key to go back to Main Menu...\n";
    system("pause>0");
    ShowMainMenu();
}

void GoBackToTransactionsMenu(stUsers& CurrentUser)
{
    cout << "\n\nPress any key to go back to Transactions Menu...\n";
    system("pause>0");
    ShowTransactionsScreen();
}

string ReadClientAccountNumber()
{
    string AccountNumber;
    cout << "\nPlease Enter Account Number? ";
    cin >> AccountNumber;
    return AccountNumber;
}

void PrintClientCard(const stClient& Client)
{
    cout << "\n\nThe following are the client details:\n";
    cout << "----------------------------------\n";
    cout << "Account Number : " << Client.Account_Number;
    cout << "\nPIN Code       : " << Client.PIN_Code;
    cout << "\nName           : " << Client.Client_Name;
    cout << "\nPhone          : " << Client.Client_Phone;
    cout << "\nAccount Balance: " << Client.Account_Balance;
    cout << "\n----------------------------------\n";
}

// ==========================================
// Main Screen Handlers
// ==========================================

void ShowClientListHeader(const vector<stClient>& vClients)
{
    cout << "\t\t\t\t\tClient List (" << vClients.size() << ") Client(s).\n";
    cout << "|-----------------------------------------------------------------------------------------------------------|\n";
    cout << "|" << left << setw(16) << " Account Number"
        << "|" << left << setw(16) << " PIN Code"
        << "|" << left << setw(26) << " Client Name"
        << "|" << left << setw(24) << " Client Phone"
        << "|" << left << setw(21) << " Account Balance"
        << "|\n";
    cout << "|___________________________________________________________________________________________________________|\n";
}

void ShowClientListBody(const vector<stClient>& vClients)
{
    for (const stClient& C : vClients)
    {
        cout << "| " << left << setw(15) << C.Account_Number
            << "| " << left << setw(15) << C.PIN_Code
            << "| " << left << setw(25) << C.Client_Name
            << "| " << left << setw(23) << C.Client_Phone
            << "| " << left << setw(20) << C.Account_Balance << "|\n";
    }
    cout << "|___________________________________________________________________________________________________________|\n\n\n";
}

void PrintAccessDeniedMessage()
{
    cout << "\n---------------------------------\n";
    cout << "Access Denied,\n";
    cout << "You dont have Permission To Do this,\n";
    cout << "Please Conat Your Admen\n";
    cout << "\n---------------------------------\n";
}

void ShowClientsList()
{
    vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
    ShowClientListHeader(vClients);
    ShowClientListBody(vClients);
}

void ShowAllClientScreen()
{

    if (!IsUserHasPermissions(enPermissions::eShow_Client_List))
    {
        PrintAccessDeniedMessage();
        return;
    }

    ShowClientsList();
}

stClient ReadNewClient()
{
    stClient Client;
    cout << "Enter Account Number? ";
    getline(cin >> ws, Client.Account_Number);

    while (ClientExistsByAccountNumber(ClientsFileName, Client.Account_Number))
    {
        cout << "Client With Account Number (" << Client.Account_Number << ") Already exists, Enter another Account Number? ";
        getline(cin, Client.Account_Number);
    }

    cout << "\nEnter PIN Code: ";
    getline(cin, Client.PIN_Code);

    cout << "\nEnter Client Name: ";
    getline(cin, Client.Client_Name);

    cout << "\nEnter Client Phone: ";
    getline(cin, Client.Client_Phone);

    cout << "\nEnter Account Balance: ";
    cin >> Client.Account_Balance;

    return Client;
}

void AddClients()
{
    char More = 'N';
    do
    {
        cout << "\n\nAdding New Client:\n\n";
        stClient Client = ReadNewClient();
        AddDataLineToFile(ClientsFileName, ConvertRecordToLine(Client));

        cout << "\n\nClient Added Successfully. Do you want to add more Clients? Y/N ";
        cin >> More;
    } while (toupper(More) == 'Y');
}

void ShowAddNewClientScreen()
{

    if (!IsUserHasPermissions(enPermissions::eAdding_New_Client))
    {
        PrintAccessDeniedMessage();
        return;
    }

    cout << "---------------------------------------\n";
    cout << "\tAdd New Client Screen: \n";
    cout << "---------------------------------------\n";
    AddClients();
}

bool MarkClientToDelete(const string& AccountNumber, vector<stClient>& vClients)
{
    for (stClient& C : vClients)
    {
        if (C.Account_Number == AccountNumber)
        {
            C.MarkToDelete = true;
            return true;
        }
    }
    return false;
}

bool DeleteClientByAccountNumber(const string& AccountNumber, vector<stClient>& vClients)
{
    char Answer = 'n';
    stClient Client;
    if (FindClientRecordByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientCard(Client);

        cout << "\n\nAre you sure you want to delete this client? y/n ? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
        {
            MarkClientToDelete(AccountNumber, vClients);
            SaveClientsRecordInFile(ClientsFileName, vClients);

            //What this line for?
            vClients = LoadClientsFromFile(ClientsFileName);

            cout << "\n\nClient Deleted Successfully";
            return true;
        }
    }
    else
    {
        cout << "Client With Account Number (" << AccountNumber << ") NOT Found!.\n";
    }
    return false;
}

void DeleteClien()
{
    cout << "\n-----------------------------------\n";
    cout << "\tDelete Client Screen";
    cout << "\n-----------------------------------\n";

    vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    DeleteClientByAccountNumber(AccountNumber, vClients);
}

void ShowDeleteClientScreen()
{
    if (!IsUserHasPermissions(enPermissions::eDeletet_Client))
    {
        PrintAccessDeniedMessage();
        return;
    }

    DeleteClien();
}

stClient ChangeClientRecord(string AccountNumber)
{
    stClient Client;
    Client.Account_Number = AccountNumber;
    cout << "\nEnter PIN Code: ";
    getline(cin >> ws, Client.PIN_Code);

    cout << "\nEnter Client Name: ";
    getline(cin, Client.Client_Name);

    cout << "\nEnter Client Phone: ";
    getline(cin, Client.Client_Phone);

    cout << "\nEnter Account Balance: ";
    cin >> Client.Account_Balance;

    return Client;
}

bool UpdateClientByAccountNumber(const string& AccountNumber, vector<stClient>& vClients)
{
    stClient Client;
    char Answer = 'n';
    if (FindClientRecordByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientCard(Client);

        cout << "\n\nAre you sure you want to Update this client? y/n ? ";
        cin >> Answer;
        if (tolower(Answer) == 'y')
        {
            for (stClient& C : vClients)
            {
                if (C.Account_Number == AccountNumber)
                {
                    C = ChangeClientRecord(AccountNumber);
                    break;
                }
            }
            SaveClientsRecordInFile(ClientsFileName, vClients);
            cout << "\n\nClient Updated Successfully.\n";
            return true;
        }
    }
    else
    {
        cout << "\n\nClient with Account Number: (" << AccountNumber << ") NOT Found!.\n";
    }
    return false;
}

void ShowUpdatClientScreen()
{
    

    if (IsUserHasPermissions(enPermissions::eUpdate_Client))
    {
        cout << "\n-----------------------------------\n";
        cout << "\tUpdate Client Info Screen\n";
        cout << "-----------------------------------\n\n";

        vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
        string AccountNumber = ReadClientAccountNumber();
        UpdateClientByAccountNumber(AccountNumber, vClients);
    }
    else
    {
        cout << "\n---------------------------------\n";
        cout << "Access Denied,\n";
        cout << "You dont have Permission To Do this,\n";
        cout << "Please Conat Your Admen\n";
        cout << "\n---------------------------------\n";

    }


}

bool FindClient(const string& AccountNumber, const vector<stClient>& vClients)
{
    stClient Client;
    if (FindClientRecordByAccountNumber(AccountNumber, vClients, Client))
    {
        PrintClientCard(Client);
        return true;
    }
    else
    {
        cout << "\n\nClient with Account Number[" << AccountNumber << "] is NOT Found!.";
    }
    return false;
}

void ShowFindClientScreen()
{


    if (!IsUserHasPermissions(enPermissions::eFind_Client))
    { 
        PrintAccessDeniedMessage();
        return;
    }

    cout << "\n-----------------------------------\n";
    cout << "\tFind Client Screen\n";
    cout << "\n-----------------------------------\n";

    vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();
    FindClient(AccountNumber, vClients);
}

void ShowExitScreen()
{
    cout << "\n------------------------------------\n";
    cout << "\tProgram Ends :-)";
    cout << "\n------------------------------------\n";
}

void PrintBalancesTableHeader(const vector<stClient>& vClients)
{
    cout << "\t\t\tBalances List (" << vClients.size() << ") Client(s).\n";
    cout << "------------------------------------------------------------------------------\n";
    cout << "| " << left << setw(15) << "Account Number";
    cout << "| " << left << setw(30) << "Client Name";
    cout << "| " << left << setw(25) << "Balance";
    cout << "\n------------------------------------------------------------------------------\n";
}

void PrintBalancesTableBody(const vector<stClient>& vClients)
{
    double TotalBalances = 0;
    for (const stClient& C : vClients)
    {
        cout << "| " << left << setw(15) << C.Account_Number
            << "| " << left << setw(30) << C.Client_Name
            << "| " << left << setw(25) << C.Account_Balance << "\n";
        TotalBalances += C.Account_Balance;
    }
    cout << "------------------------------------------------------------------------------\n";
    cout << "\t\t\tTotal Balances = " << TotalBalances << endl;
}

void ShowTotalBalances()
{
    vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
    PrintBalancesTableHeader(vClients);
    PrintBalancesTableBody(vClients);
}

bool DepositBalanceClientsByAccountNumber(const string& AccountNumber, const double& Amount, vector<stClient>& vClients)
{
    char Answer = 'n';
    cout << "\nAre you sure you want to perform this transaction? y/n ? ";
    cin >> Answer;
    if (tolower(Answer) == 'y')
    {
        for (stClient& C : vClients)
        {
            if (C.Account_Number == AccountNumber)
            {
                C.Account_Balance += Amount;
                SaveClientsRecordInFile(ClientsFileName, vClients);
                cout << "\n\nDone Successfully. Account Balance is: " << C.Account_Balance << endl;
                return true;
            }
        }
    }
    else
    {
        cout << "\nTransaction Cancelled.";
    }
    return false;
}

void ShowDepositScreen()
{
    stClient Client;
    cout << "\n-------------------------------------\n";
    cout << "\tDeposit Screen";
    cout << "\n-------------------------------------\n";
    vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();

    while (!FindClientRecordByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\n\nClient with [" << AccountNumber << "] does not exist.\n";
        AccountNumber = ReadClientAccountNumber();
    }
    PrintClientCard(Client);

    double Amount = 0;
    cout << "\nPlease enter deposit amount ? ";
    cin >> Amount;
    DepositBalanceClientsByAccountNumber(AccountNumber, Amount, vClients);
}

void ShowWithDrawScreen()
{
    stClient Client;
    cout << "\n-------------------------------------\n";
    cout << "\tWithdraw Screen";
    cout << "\n-------------------------------------\n";
    vector<stClient> vClients = LoadClientsFromFile(ClientsFileName);
    string AccountNumber = ReadClientAccountNumber();

    while (!FindClientRecordByAccountNumber(AccountNumber, vClients, Client))
    {
        cout << "\nClient with Account Number [" << AccountNumber << "] is Not Found!\n";
        AccountNumber = ReadClientAccountNumber();
    }

    PrintClientCard(Client);

    double Amount = 0;
    cout << "Please Enter Withdraw Amount: ";
    cin >> Amount;

    while (Amount > Client.Account_Balance)
    {
        cout << "\nAmount Exceeds the balance, you can Withdraw up to : " << Client.Account_Balance;
        cout << "\nPlease Enter Withdraw Amount: ";
        cin >> Amount;
    }
    DepositBalanceClientsByAccountNumber(AccountNumber, Amount * -1, vClients);
}



// ==========================================
//Manage Users Options.
// ==========================================



string ConvertUserRecordToLine(stUsers NewUser, string Separator = "#//#")
{
    string UserLine;

    UserLine += NewUser.UserName + Separator;
    UserLine += NewUser.User_Password + Separator;
    UserLine += to_string(NewUser.Permissions);


    return UserLine;
}

stUsers ConvertUserLineToRecord(string Line)
{
    vector <string> vUser = SplitLine(Line);

    stUsers User;

    if (vUser.size() >= 3)
    {
        User.UserName = vUser[0];
        User.User_Password = vUser[1];
        User.Permissions = stoi(vUser[2]);
    }

    return User;
}

vector <stUsers> LoadUserFromFile(const string& FileName)
{
    vector <stUsers> vUser;
    fstream UsersFile;

    UsersFile.open(FileName, ios::in);//Read Mode

    if (UsersFile.is_open())
    {
        string Line;

        while (getline(UsersFile, Line))
        {
            if (Line != "")
            {
                vUser.push_back(ConvertUserLineToRecord(Line));
            }
        }
        UsersFile.close();
    }

    return vUser;
}


short ReadManageUserChoice()
{
    short Choice = 0;

    cout << "\nChoose What you want to do?  [1 to 6]? ";
    cin >> Choice;
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return Choice;
}



void ShowUsersListHeader(const vector <stUsers>& vUsers)
{
    cout << "\t\t\tUser List (" << vUsers.size() << "User(s)\n";
    cout << "----------------------------------------------------------------------\n";
    cout << "| " << left << setw(16) << "Username"
        << "| " << left << setw(18) << "Passowrd"
        << "| " << left << setw(18) << "Permissions";

    cout << "\n----------------------------------------------------------------------\n";
}

void ShowUserListBody(const vector <stUsers>& vUsers)
{
    for (const stUsers& U : vUsers)
    {
        cout << "| " << left << setw(16) << U.UserName
            << "| " << left << setw(18) << U.User_Password
            << "| " << left << setw(16) << U.Permissions << "\n";
    }

    cout << "----------------------------------------------------------------------\n";

}

void ShowUsersListScreen()
{
    vector <stUsers> vUsers = LoadUserFromFile("Users.txt");
    ShowUsersListHeader(vUsers);
    ShowUserListBody(vUsers);

}



bool UserExistByUsername(const string UserName, string UserFileName)
{
    vector <stUsers> vUser = LoadUserFromFile(UsersFileName);

    for (const stUsers& U : vUser)
    {
        if (U.UserName == UserName)
        {
            return true;
        }
    }
    return false;
}

int GetUserPermissions(stUsers Current_User)
{
    vector <stUsers> vUsers = LoadUserFromFile(UsersFileName);

    for (stUsers& U : vUsers)
    {
        if (U.UserName == Current_User.UserName)
            return U.Permissions;
    }

    return 0;
}

bool FindUserByuserNameAndPassword(string UserName, string Password, stUsers& User)
{
    vector <stUsers> vUser = LoadUserFromFile(UsersFileName);


    for (stUsers& U : vUser) 
    {
        if (U.UserName == UserName   && U.User_Password == Password  )
        {
            User = U;
            return true;
        }
    }

    return false;
}

bool LoadUserInfo(string UserName, string Password)
{
    if (FindUserByuserNameAndPassword(UserName, Password, CurrentUser))
        return true;
    else
        return false;
    
}


bool IsUserHasPermissions(enPermissions PermissionsRequired)
{

    if (CurrentUser.Permissions == enPermissions::All)
    {
        return true;
    }

    if ((CurrentUser.Permissions & PermissionsRequired) == PermissionsRequired)
        return true;
    else
    return false;
}


int ReadUserPermissions()
{
    int User_Permissons = 0;
    char Answer;

    cout << "\nDo you want to give full access? [y/n]?  ";
    cin >> Answer;

    if (Answer == 'y')
    {
        User_Permissons = -1;
        return User_Permissons;
    }


    cout << "\nShow Client List? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 1;
    }


    cout << "\nAdd New Clent? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 2;
    }


    cout << "\nDelete Client? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 4;
    }


    cout << "\n Update Clinet? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 8;
    }

    cout << "\nFind Client? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 16;
    }

    cout << "\nTransactions? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 32;
    }

    cout << "\nManage Users? [y/n]?  ";
    cin >> Answer;
    if (Answer == 'y')
    {
        User_Permissons += 64;
    }

    return User_Permissons;
}

stUsers ReadUserInfo()
{
    stUsers User;

    cout << "Enter Username? ";
    getline(cin >> ws, User.UserName);

    while (UserExistByUsername(User.UserName, UsersFileName))
    {
        cout << "\nUser With [" << User.UserName << "] already exists, Enter another Username?  ";
        getline(cin >> ws, User.UserName);
    }
    cout << "\nEnter Password? ";
    cin >> User.User_Password;

    User.Permissions = ReadUserPermissions();

    return User;
}


void AddNewUser()
{
    stUsers User;

   User = ReadUserInfo();
   AddDataLineToFile(UsersFileName, ConvertUserRecordToLine(User));
}

void AddingNewUsers()
{
    char More = 'n';

    do 
    {
        cout << "Adding New User:\n\n";

        AddNewUser();

        cout << "\nUser Added Successfully, do you want to add more Users? [y/n]?  ";
        cin >> More;

    } while (tolower(More) == 'y');
}

void ShowAddNewUserScreen()
{
    cout << "-------------------------------------\n";
    cout << "\tAdd New User Screen\n";
    cout << "-------------------------------------\n";

    AddingNewUsers();
}



string ReadUsername()
{
    string Username;


    cout << "Please enter username?  ";
    getline(cin >> ws, Username);

    return Username;
}

bool FindUserByUsername(string Username, vector <stUsers> &vUsers, stUsers& User)
{

    for (const stUsers& U : vUsers)
    {
        if (U.UserName == Username)
        {
            User = U;
            return true;
        }
    }
    return false;
}

void PrintUserCard(stUsers User)
{
    cout << "\nthe following are the User detailes:";
    cout << "\n----------------------------------";
    cout << "\nUsername     : " << User.UserName;
    cout << "\nPassword     : " << User.User_Password;
    cout << "\nPermissions  : " << User.Permissions;
    cout << "\n----------------------------------";

}

bool MarkUserToDelete(const string &Username, vector <stUsers> &vUsers)
{
    for (stUsers& U : vUsers)
    {
        if (U.UserName == Username)
        {
            U.MarkToDelete = true;
            return true;
        }
    }

    return false;
}

void SaveUserRecordToFil(string FilName, vector <stUsers>& vUsers)
{
    fstream MyFile;

    MyFile.open(FilName, ios::out);//writ mode.

    if (MyFile.is_open())
    {
        for (stUsers& U : vUsers)
        {
            if (!U.MarkToDelete)
                MyFile << ConvertUserRecordToLine(U) << "\n";

        }
        MyFile.close();
    }
}

void DeleteUserByUsername(const string& Username, vector <stUsers>& vUsers)
{
    stUsers User;
    char Anser = 'n';

    if (FindUserByUsername(Username, vUsers, User))
    {
        PrintUserCard(User);


        cout << "\nAre you sure you want to delete this User?  y/n?  ";
        cin >> Anser;
        if (tolower(Anser) == 'y')
        {
            MarkUserToDelete(Username, vUsers);
            SaveUserRecordToFil(UsersFileName, vUsers);
        }

    }
    else
    {
        cout << "\nUser with Username: " << Username << " NOT Found\n";
    }
}

void showDeleteUserHeader()
{
    cout << "------------------------------\n";
    cout << "\tDelete User Screen\n";
    cout << "------------------------------\n";

}

void showDeleteUserScreen(stUsers CurrentUser)
{
    showDeleteUserHeader();

    vector <stUsers> vUsers = LoadUserFromFile(UsersFileName);
    string Username = ReadUsername();
    if (CurrentUser.UserName == Username)
    {
        cout << "\nYou can not delete your User account\n";
    }
    else
    {
        DeleteUserByUsername(Username, vUsers);
    }
}



void ShowUpdatUserHeader()
{
    cout << "-----------------------------------\n";
    cout << "\tShow Updat User\n";
    cout << "-----------------------------------\n";
}

stUsers ChangUserRecord(string Username)
{
    stUsers User; 
    User.UserName = Username;
    cout << "\nEnter Password:  ";
    cin >> User.User_Password;

    User.Permissions = ReadUserPermissions();

    return User;
}

bool UpdatUserInfo(const string& Username, vector <stUsers> &vUsers)
{
    char Answer = 'n';
    stUsers User;

    if (FindUserByUsername(Username, vUsers, User))
    {
        PrintUserCard(User);

        cout << "\nAre sure you want to update user: " << User.UserName << " ?  y/n?  ";
        cin >> Answer;

        if (tolower(Answer) == 'y')
        {
            for (stUsers& U : vUsers)
            {
                if (U.UserName == Username)
                {
                    U = ChangUserRecord(Username);
                }
            }

            SaveUserRecordToFil(UsersFileName, vUsers);
            cout << "\nUser Updaded Successfully\n";
            return true;
        }
    }
    else
    {
        cout << "\nUser with Username: " << Username << " NOT found!\n";
    }
    return false;
}

void ShowUpdatUserScreen()
{
    ShowUpdatUserHeader();

    vector <stUsers> vUsers = LoadUserFromFile(UsersFileName);
    string Username = ReadUsername();
    UpdatUserInfo(Username, vUsers);
}



bool FindUser(string Username, vector <stUsers>& vUsers)
{
    stUsers User;
    if (FindUserByUsername(Username, vUsers, User))
    {
        PrintUserCard(User);

        return true;
    }
    else
    {
        cout << "\nUser With Username: " << Username << " Not found\n";
    }

    return false;
}

void ShowFindUserScreen()
{
    cout << "---------------------------------------\n";
    cout << "\tFind User Screen\n";
    cout << "---------------------------------------\n";


    vector <stUsers> vUsers = LoadUserFromFile(UsersFileName);
    string Username = ReadUsername();
    FindUser(Username, vUsers);
}

void GoBackToManageUsersMenue(stUsers CurrentUser)
{
    cout << "\nPrass any key to back to Manage Users Menue...\n";
    system("pause>0");
    ShowManageUserScreen();
    
}

void PerformManageUsersMenueOption(enManageUser ManageUser)
{
    switch (ManageUser)
    {
        case enManageUser::eListUser:
            system("cls");
            ShowUsersListScreen();
            GoBackToManageUsersMenue(CurrentUser);
            break;
            
        case enManageUser::eAdd_New_User:
            system("cls");
            ShowAddNewUserScreen();
            GoBackToManageUsersMenue(CurrentUser);
            break;

        case enManageUser::eDelete_User:
            system("cls");
            showDeleteUserScreen(CurrentUser);
            GoBackToManageUsersMenue(CurrentUser);
            break;

        case enManageUser::eUpdate_User:
            system("cls");
            ShowUpdatUserScreen();
            GoBackToManageUsersMenue(CurrentUser);
            break;

        case enManageUser::eFind_User:
            system("cls");
            ShowFindUserScreen();
            GoBackToManageUsersMenue(CurrentUser);
            break;


        case enManageUser::eMain_Menue:
            system("cls");
            ShowMainMenu();
            break;
    }
}

void ShowManageUserScreen()
{
    if (!IsUserHasPermissions(enPermissions::eManage_Users))
    {
        PrintAccessDeniedMessage();
        return;
    }
    
    system("cls");
    cout << "================================================\n";
    cout << "\tManage Users Menue Screen\n";
    cout << "================================================\n";
    cout << "\t[1] List User.\n";
    cout << "\t[2] Add New Uswer.\n";
    cout << "\t[3] Delete User.\n";
    cout << "\t[4] Update User.\n";
    cout << "\t[5] Find User\n";
    cout << "\t[6] Main Menue\n";
    cout << "================================================\n";

    PerformManageUsersMenueOption((enManageUser)ReadManageUserChoice());
    
}


void LoginScreen()
{
    bool LoginFaild = false;
    string Username, Password;

    do 
    {
        system("cls");

        cout << "\n--------------------------------------\n";
        cout << "\tLogin Screen";
        cout << "\n--------------------------------------\n";

        if (LoginFaild)
        {
            cout << "Invalid Username/Password!\n";
        }

        cout << "Enter Username?  ";
        cin >> Username;
        cout << "Enter password?  ";
        cin >> Password;

        LoginFaild = !LoadUserInfo(Username, Password);

    } while (LoginFaild);
            

    ShowMainMenu();

}

// ==========================================



short ReadTransactionOption()
{
    short Trans = 0;
    cout << "Choose what do you want to do? [1 to 4]? ";
    cin >> Trans;
    return Trans;
}

void PerformTransactionsOption(enTransactions Choose)
{
    switch (Choose)
    {
    case enTransactions::eDeposit:
        system("cls");
        ShowDepositScreen();
        GoBackToTransactionsMenu(CurrentUser);
        break;

    case enTransactions::eWithdraw:
        system("cls");
        ShowWithDrawScreen();
        GoBackToTransactionsMenu(CurrentUser);
        break;

    case enTransactions::eTotalBalances:
        system("cls");
        ShowTotalBalances();
        GoBackToTransactionsMenu(CurrentUser);
        break;

    case enTransactions::eMainMenu:
        system("cls");
        ShowMainMenu();
        break;
    }
}

void ShowTransactionsScreen()
{
    if (!IsUserHasPermissions(enPermissions::eTransactions))
    {
        PrintAccessDeniedMessage();
        return;
    }

    system("cls");
    cout << "================================================\n";
    cout << "\t\tTransaction Menu Screen\n";
    cout << "================================================\n";
    cout << "\t[1] Deposit.\n";
    cout << "\t[2] Withdraw.\n";
    cout << "\t[3] Total Balances.\n";
    cout << "\t[4] Main Menu.\n";
    cout << "================================================\n";
    PerformTransactionsOption((enTransactions)ReadTransactionOption());
 
}

short ReadMainMenuOption()
{
    short Option = 0;
    cout << "Choose what do you want to do? [1 To 8]? ";
    cin >> Option;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');


    return Option;
}

void PerformMainMenuOption(enMenuOption Choose, stUsers CurrentUser)
{
    switch (Choose)
    {
    case enMenuOption::Show_Client_List:
        system("cls");
        ShowAllClientScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::Add_New_Client:
        system("cls");
        ShowAddNewClientScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::Delete_Client:
        system("cls");
        ShowDeleteClientScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::Update_ClientInfo:
        system("cls");
        ShowUpdatClientScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::Find_Client:
        system("cls");
        ShowFindClientScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::Transactions:
        system("cls");
        ShowTransactionsScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::Manage_User:
        system("cls");
        ShowManageUserScreen();
        GoBackToMainMenu();
        break;

    case enMenuOption::LogOut:
        system("cls");
        LoginScreen();
        break;
    }
}

void ShowMainMenu()
{
    system("cls");
    cout << "============================================\n";
    cout << "\t\tMain Menu Screen\n";
    cout << "============================================\n";
    cout << "\t[1] Show Client List.\n";
    cout << "\t[2] Add New Client.\n";
    cout << "\t[3] Delete Client.\n";
    cout << "\t[4] Update Client Info.\n";
    cout << "\t[5] Find Client.\n";
    cout << "\t[6] Transactions.\n";
    cout << "\t[7] Manage User.\n";
    cout << "\t[8] Logout.\n";
    cout << "============================================\n";

    PerformMainMenuOption((enMenuOption)ReadMainMenuOption(), CurrentUser);
}



int main()
{

    LoginScreen();

    return 0;
}
