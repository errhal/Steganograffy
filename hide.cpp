#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

int write(char **params);
int read(char **params);
char *encode(char *text, long text_size, char *password);

int main(int argc, char **argv) {

    if (!((argc == 6 && strcmp(argv[1], "write") == 0) || (argc == 4 && strcmp(argv[1], "read") == 0))) {
        cout << "Usage of program: \n"
                "For hiding text: ./program write input.jpg output.jpg secret_text secret_key\n"
                "For reading text: ./program read image.jpg secret_key\n";
        return -1;
    }
    if (strcmp(argv[1], "write") == 0) write(argv);
    else read(argv);
    return 0;
}


int write(char **params) {
    long text_size = strlen(params[4]);
    char *text = encode(params[4], strlen(params[4]), params[5]);
    ifstream input;
    ofstream output;

    input.open(params[2], ios::in | ios::binary);
    output.open(params[3], ios::out | ios::binary);
    if (!input.is_open()) {
        cout << "Error reading the file!\n";
        return -1;
    }

    char mem_dump[2], last_char = 0x00;

    int current_text_pos = 0;

    while (!input.eof()) {
        input.read(mem_dump, 1);
        if (last_char == '\xFF' && mem_dump[0] == '\xD6' && current_text_pos < text_size) {
            output.seekp(output.tellp() - 2);
            output.write(&text[current_text_pos++], sizeof(char));
            output.write("\xFF", 1);
        }
        last_char = mem_dump[0];
        output.write(mem_dump, 1);
    }
}

int read(char **params) {
    ifstream input;
    long text_size = 0;
    string secret_key = params[3];
    input.open(params[2], ios::in | ios::binary);
    if (!input.is_open()) {
        cout << "Error reading the file!\n";
        return -1;
    }

    char current, last = 0x00;
    string secret_text;

    while (!input.eof()) {
        current = (char) input.get();
        if (last == '\xFF' && current == '\xD6') {
            text_size++;
            input.seekg(input.tellg() - 3);
            secret_text += input.get();
            input.seekg(input.tellg() + 2);
        }
        last = current;
    }
    cout << encode((char *) secret_text.c_str(), text_size, (char *) secret_key.c_str()) << endl;
}

char *encode(char *text, long text_size, char *password) {
    long password_size = strlen(password);
    char *encoded_password = new char(sizeof(char) * (text_size + 1));

    for (int i = 0; i < text_size; i++) {
        encoded_password[i] = text[i] ^ password[i % password_size];
    }
    encoded_password[text_size] = 0x00;
    return encoded_password;
}
