CC = gcc
HEADER_DIR = include
CFLAGS = -Werror -I$(HEADER_DIR) -g
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

LIBS = 

# Récupérer tous les fichiers .c dans src/ sauf main.c
LIB_SRCS = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
# Générer les noms des fichiers objets correspondants
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))

all: $(BIN_DIR)/main 

# Créer les répertoires nécessaires
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Compiler chaque fichier source de librairie en fichier objet
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

# Linker le main avec tous les fichiers objets des librairies
$(BIN_DIR)/main: $(SRC_DIR)/main.c $(LIB_OBJS) | $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

run: $(BIN_DIR)/main
	@./$(BIN_DIR)/main

clean:
	@rm -rf "$(BIN_DIR)/main" "$(OBJ_DIR)"

.PHONY: all run clean
