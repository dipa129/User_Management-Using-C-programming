#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#endif

#define MAX_USERS 10
#define CREDENTIAL_LENGTH 30

typedef struct {
    char username[CREDENTIAL_LENGTH];
    char password[CREDENTIAL_LENGTH];
} User;

User users[MAX_USERS];
int user_count = 0;

void register_user();
int login_user();    // Returns the user index
void fix_fgets_input(char* string);
void input_credentials(char* username, char* password);

int main() {
    int option;
    int user_index;

    while (1) {
        printf("\nWelcome to User Management:");
        printf("\n1. Register");
        printf("\n2. Login");
        printf("\n3. Exit");
        printf("\nSelect an option: ");
        scanf("%d", &option);
        getchar();  // Consume the extra newline character

        switch (option) {
            case 1:
                register_user();
                break;
            case 2:
                user_index = login_user();
                if (user_index >= 0) {
                    printf("Login successful! Welcome, %s!\n", users[user_index].username);
                } else {
                    printf("\nLogin failed! Incorrect username or password.\n");
                }
                break;
            case 3:
                printf("\nExiting program.\n");
                return 0;
            default:
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    }
    return 0;
}

void register_user() {
    if (user_count == MAX_USERS) {
        printf("\nMaximum %d users are supported! No more registrations allowed!\n", MAX_USERS);
        return;
    }

    int new_index = user_count;
    printf("\nRegister a new user:");
    printf("\nEnter Username: ");
    fgets(users[new_index].username, CREDENTIAL_LENGTH, stdin);
    fix_fgets_input(users[new_index].username);

    printf("Enter Password (masking enabled): ");
    input_credentials(NULL, users[new_index].password);

    user_count++;
    printf("\nRegistration successful!\n");
}

int login_user() {
    char username[CREDENTIAL_LENGTH];
    char password[CREDENTIAL_LENGTH];

    printf("\nEnter Username: ");
    fgets(username, CREDENTIAL_LENGTH, stdin);
    fix_fgets_input(username);

    printf("Enter Password (masking enabled): ");
    input_credentials(NULL, password);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(username, users[i].username) == 0 && strcmp(password, users[i].password) == 0) {
            return i;
        }
    }

    return -1;
}

void input_credentials(char* username, char* password) {
    if (username != NULL) {
        printf("\nEnter Username: ");
        fgets(username, CREDENTIAL_LENGTH, stdin);
        fix_fgets_input(username);
    }

#ifdef _WIN32
    // Windows platform: Use _getch() for password masking
    char ch;
    int i = 0;
    while ((ch = _getch()) != '\r') {  // Wait for Enter key (carriage return)
        if (ch == '\b' && i > 0) {  // Handle backspace
            i--;
            printf("\b \b");
        } else if (i < CREDENTIAL_LENGTH - 1) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
#else
    // Unix-like platforms: Use termios to disable echo for password input
    struct termios old_props, new_props;
    tcgetattr(STDIN_FILENO, &old_props);
    new_props = old_props;
    new_props.c_lflag &= ~(ECHO);  // Disable echoing
    new_props.c_lflag |= ICANON;  // Ensure canonical input mode
    tcsetattr(STDIN_FILENO, TCSANOW, &new_props);

    int i = 0;
    char ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        if ((ch == '\b' || ch == 127) && i > 0) {  // Handle backspace
            i--;
            printf("\b \b");
        } else if (i < CREDENTIAL_LENGTH - 1) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';

    tcsetattr(STDIN_FILENO, TCSANOW, &old_props);  // Restore terminal properties
#endif

    printf("\n");
}

void fix_fgets_input(char* string) {
    int index = strcspn(string, "\n");
    if (index < CREDENTIAL_LENGTH) {
        string[index] = '\0';  // Remove newline character
    }
}
