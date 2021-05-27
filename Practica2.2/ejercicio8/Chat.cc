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

    memcpy(pointer, &nick, 8);
    pointer += 8;
    
    memcpy(pointer, &message, 80);

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data

    char* pointer = _data;

    memcpy(&type, pointer, sizeof(uint8_t));
    pointer += sizeof(uint8_t);

    memcpy(&nick, pointer, 8);
    pointer += 8;
    
    memcpy(&message, pointer,80);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    clients = std::vector<std::unique_ptr<Socket>>();
    clients.reserve(5);
    
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
    
        ChatMessage msg = ChatMessage();

        Socket *sd = 0;
        socket.recv(msg, sd);

        std::cout << sd << std::endl;

        switch (msg.type)
        {
            case ChatMessage::MessageType::LOGIN:{
                std::cout << "LOGIN\n" << std::flush;
                clients.push_back(std::unique_ptr<Socket>(std::move(sd)));
                sd = nullptr;
                break;
            }
            case ChatMessage::MessageType::MESSAGE:
                std::cout << "MESSAGE\n" << std::flush;
                for(auto it = clients.begin(); it != clients.end(); ++it){
                    if(it->get() == sd)
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
                std::cout << "UNKOWN\n" << std::flush;
                break;
        }
        std::cout << "out\n" << std::flush;
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::MessageType::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    // Completar
    std::string msg;

    ChatMessage em(nick, "");
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

        ChatMessage em(nick, msg);
        em.type = ChatMessage::MessageType::MESSAGE;

        socket.send(em, socket);
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage em;
        socket.recv(em);
        std::cout << em.nick << ": " << em.message << ".\n";
    }
}

