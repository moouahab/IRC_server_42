NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g3 -O3 -std=c++98
OBJDIR = obj
SRCS = CapCommand.cpp Client.cpp CommandHandler.cpp NickCommand.cpp PassCommand.cpp \
       PingCommand.cpp PrivmsgCommand.cpp Server.cpp UserCommand.cpp main.cpp utils.cpp\
	   WhoisCommand.cpp

OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)  # Fichiers de dépendances

all: $(NAME)

# Créer le répertoire obj si ce n'est pas déjà fait
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Compile les fichiers objets et génère les fichiers de dépendance en même temps
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

# Lien final pour créer l'exécutable
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# Inclure les fichiers de dépendance pour recompilation automatique en cas de changement d'inclusion
-include $(DEPS)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all
