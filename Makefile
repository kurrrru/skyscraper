#
# Compiler and Flags
#
CXX			= g++
CXXFLAGS	= -Wall -Wextra -Werror -O3 -std=c++11

#
# Project Variables
#
NAME		= solver
SRCS		= src/main.cpp
OBJS		= $(SRCS:.cpp=.o)
RM			= rm -f

#
# Rules
#
.DEFAULT_GOAL := all

all:		$(NAME)

# Linking
$(NAME):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

# Compilation
%.o:		%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test generation
gen-%:	
	python3 src/gn.py $*

# Clean rules
clean:
	$(RM) $(OBJS)

fclean:		clean
	$(RM) $(NAME)

re:			fclean all

# Phony targets
.PHONY:		all clean fclean re
