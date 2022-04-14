
EXE = tcp_proxy

SRC =	tcp_proxy.cpp \
				ProxyServer.cpp \
				FdHandler.cpp \
				EventSelector.cpp \
				Session.cpp \
				TcpBridge.cpp \
				utilities.cpp

OBJ = $(SRC:.cpp=.o)

CXX = clang++ -std=c++11

CXXFLAGS = -Wall -Wextra -Werror

RM = rm -rf

COMPLETE_MSG = @echo 'Build is complete. Run ./tcp_proxy <local_port> <forward_ip> <forwawrd_port>'

all: $(EXE) clean complete

$(EXE): $(SRC) *.hpp $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXE)


clean:
	@echo 'Object files have been deleted'`$(RM) $(OBJ)`''

fclean: clean
	@echo 'Binary has been deleted'`$(RM) $(EXE)`''

re: fclean all

complete:
	$(COMPLETE_MSG)

.PHONY: all clean fclean re