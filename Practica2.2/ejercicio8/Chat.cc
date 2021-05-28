#include "Chat.h"

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data

    char* pointer = _data;

    memcpy(pointer, &type, sizeof(uint8_t));
    pointer += sizeof(uint8_t);

    memcpy(pointer, nick.c_str(), 8);
    pointer += 8;
    
    memcpy(pointer, message.c_str(), 80);

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data

    char* pointer = bobj;

    memcpy(&type, pointer, sizeof(uint8_t));
    pointer += sizeof(uint8_t);

    char _nick_[8];

    memcpy(&_nick_, pointer, 8);
    pointer += 8;
    nick = _nick_;
    
    char _msg_[80];

    memcpy(&_msg_, pointer,80);
    message = _msg_;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    clients = std::vector<std::unique_ptr<Socket>>();
    clients.reserve(5);
    
    ChatMessage msg;
    while (true) {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        Socket *sd = (Socket*)1;
        socket.recv(msg, sd);

        switch (msg.type)
        {
            case ChatMessage::MessageType::LOGIN:{
                std::cout << "LOGIN\n" << std::flush;
                clients.push_back(std::unique_ptr<Socket>(std::move(sd)));
                break;
            }
            case ChatMessage::MessageType::MESSAGE:
                std::cout << "MESSAGE\n" << std::flush;
                for(auto it = clients.begin(); it != clients.end(); ++it){
                    if(*it->get() == *sd)
                        continue;
                    socket.send(msg, *it->get());
                }
                break;
            case ChatMessage::MessageType::LOGOUT: {
                std::cout << "LOGOUT\n" << std::flush;
                auto it = clients.begin();
                while( it != clients.end() ){
                    if( it->get() == sd ){
                        clients.erase(it);
                        break;
                    }
                    ++it;
                }
                break;
            }
            default:
                std::cout << "UNKOWN MESSAGE\n" << std::flush;
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    ChatMessage em("", "");
    em.type = ChatMessage::MessageType::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    ChatMessage em("", "");
    em.type = ChatMessage::MessageType::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string msg;

        std::getline(std::cin, msg);

        if(msg == "q"){
            logout();
            break;
        }

        ChatMessage em(nick, msg);
        em.type = ChatMessage::MessageType::MESSAGE;

        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    ChatMessage em;
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        socket.recv(em);
        std::cout << em.nick << ": " << em.message << ".\n";
    }
}

