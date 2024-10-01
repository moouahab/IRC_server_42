# include <algorithm> // Pour std::remove
# include <iostream>
# include <cctype>
# include <vector>
# include <string>
# include <sstream>
# include <cstring>
# include <cstdlib> // pour strtol

// Vérifie si une chaîne est un nombre
bool isNumber(const std::string &str)
{
    for (size_t i = 0; i < str.size(); i++)
        if (!std::isdigit(str[i]))
            return (false);
    return (true);
}

// Vérifie si un caractère est un underscore ou un tiret
bool isUnderscoreOrHyphen(char c)
{
    return c == '_' || c == '-';
}

// Compte le nombre de majuscules dans une chaîne
int countUpper(const std::string &str)
{
    int count = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (std::isupper(str[i]))
            count++;
    }
    return count;
}

// Compte le nombre de minuscules dans une chaîne
int countLower(const std::string &str)
{
    int count = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (std::islower(str[i]))
            count++;
    }
    return count;
}

// Compte le nombre de chiffres dans une chaîne
int countDigit(const std::string &str)
{
    int count = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (std::isdigit(str[i]))
            count++;
    }
    return count;
}

// Compte le nombre de underscores ou tirets dans une chaîne
int countUnderscoreOrHyphen(const std::string &str)
{
    int count = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (isUnderscoreOrHyphen(str[i]))
            count++;
    }
    return count;
}

// Valide le mot de passe selon les critères : 1 majuscule, 1 minuscule, 1 chiffre, et 1 tiret ou underscore
bool passwordValid(const std::string &str)
{
    if (str.size() < 8) return false;  // Au moins 8 caractères
    if (countUpper(str) < 1) return false;  // Au moins une majuscule
    if (countLower(str) < 1) return false;  // Au moins une minuscule
    if (countDigit(str) < 1) return false;  // Au moins un chiffre
    if (countUnderscoreOrHyphen(str) < 1) return false;  // Au moins un _ ou -
    return true;
}

// Parse les arguments et valide le port et le mot de passe
bool parseArgument(int &port, std::string &password, char *av[])
{
    char *endptr;
    long parsedPort = std::strtol(av[1], &endptr, 10); // Conversion en long avec strtol
    if (*endptr != '\0' || parsedPort < 1024 || parsedPort > 65535) // Vérifier que le port est valide
    {
        std::cerr << "Invalid port number. It should be between 1024 and 65535." << std::endl;
        return false;
    }
    port = static_cast<int>(parsedPort); // On cast en int après validation

    password = av[2];
    if (password.find('\n') != std::string::npos) // Vérifier s'il y a un retour à la ligne
    {
        std::cerr << "Password should not contain newline character." << std::endl;
        return false;
    }

    if (!passwordValid(password)) // Valider le mot de passe
    {
        std::cerr << "Password is not valid. It should contain at least 8 characters, "
                  << "1 uppercase letter, 1 lowercase letter, 1 digit, and 1 '_' or '-'." << std::endl;
        return false;
    }

    return true;
}

std::string removeEOT(const std::string& str) {
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), '\x04'), result.end());
    return result;
}

std::string trim(const std::string& str) {
    // Créer une copie de la chaîne d'entrée
    std::string result = str;

    // Supprimer tous les caractères EOT de la chaîne
    result.erase(std::remove(result.begin(), result.end(), '\x04'), result.end());

    // Supprimer les espaces, tabulations et retours à la ligne au début et à la fin
    size_t first = result.find_first_not_of(" \r\n\t");
    size_t last = result.find_last_not_of(" \r\n\t");
    if (first == std::string::npos || last == std::string::npos) {
        return ""; // Si la chaîne est composée uniquement d'espaces ou de retours à la ligne
    }
    return result.substr(first, last - first + 1);
}

// Function to split a string using a delimiter
std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        // if (std::isspace(delimiter))
        token = trim(token); // Supprimer les espaces avant d'ajouter le token
        if (!token.empty()) {  // Ne pas ajouter de tokens vides
            tokens.push_back(token);
        }
    }
    return tokens;
}

bool matchWildcard(const std::string& pattern, const std::string& str) {
    // Implémentation simple pour le joker '*'
    size_t pos = pattern.find('*');
    if (pos == std::string::npos) {
        return pattern == str;
    } else {
        std::string prefix = pattern.substr(0, pos);
        std::string suffix = pattern.substr(pos + 1);
        return str.substr(0, prefix.size()) == prefix &&
               str.substr(str.size() - suffix.size()) == suffix;
    }
}