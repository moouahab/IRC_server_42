# Nom de l'exécutable
NAME = ircserv

# Compilateur et options de compilation
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g3 -O3 -std=c++98 -I srcs/ -I srcs/command/ -I srcs/option/

# Répertoire pour les fichiers objets
OBJDIR = obj

# Liste des fichiers sources avec chemins relatifs corrects
SRCS = \
	srcs/command/CapCommand.cpp \
	srcs/Channel.cpp \
	srcs/Client.cpp \
	srcs/command/CommandHandler.cpp \
	srcs/command/JoinCommand.cpp \
	srcs/command/NickCommand.cpp \
	srcs/command/PassCommand.cpp \
	srcs/command/PingCommand.cpp \
	srcs/command/PartCommand.cpp \
	srcs/command/ModeCommand.cpp \
	srcs/command/PrivmsgCommand.cpp \
	srcs/command/WhoCommand.cpp \
	srcs/command/WhoisCommand.cpp \
	srcs/option/OptionInviteOnly.cpp \
	srcs/command/InviteCommand.cpp\
	srcs/command/KickCommand.cpp\
	srcs/command/TopicCommand.cpp\
	srcs/option/OptionKey.cpp \
	srcs/option/OptionLimit.cpp \
	srcs/option/OptionOperator.cpp \
	srcs/option/OptionTopicRestriction.cpp \
	srcs/Server.cpp \
	srcs/command/UserCommand.cpp \
	srcs/Logger.cpp\
	srcs/main.cpp \
	srcs/utils.cpp

# Transformation des fichiers sources en fichiers objets
# Exemple : srcs/command/CapCommand.cpp -> obj/command/CapCommand.o
OBJS = $(SRCS:srcs/%.cpp=$(OBJDIR)/%.o)

# Fichiers de dépendances
DEPS = $(OBJS:.o=.d)

# Cible par défaut
all: $(NAME)

# Règle pour lier les objets et créer l'exécutable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Règle générique pour compiler les fichiers objets
# Elle crée le répertoire nécessaire avant la compilation
$(OBJDIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Inclure les fichiers de dépendance générés
-include $(DEPS)

# Nettoyage des fichiers objets et des dépendances
clean:
	rm -rf $(OBJDIR)

# Nettoyage complet (objets + exécutable)
fclean: clean
	rm -f $(NAME)
	rm server.log

# Recompilation complète
re: fclean all

# Phony targets pour éviter les conflits avec des fichiers réels
.PHONY: all clean fclean re
