#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

#define DEFAULT_PORT "5555"
#define DEFAULT_BUFLEN 1024

class Cliente{
public:
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    SOCKADDR_IN addr;
    char buffer[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    int iResult;
    bool conectado = false;

    Cliente(const string &direccionIp, int puerto)
    {
        cout<<"Conectando al servidor."<<endl<<endl;
        iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup ha fallado con error: %d\n", iResult);
            system("pause");
        }
        
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
        //Checkeo que el Socket es Valido
        if (ConnectSocket == INVALID_SOCKET) {
            printf("Error al llamar a socket(): %ld\n", WSAGetLastError());
            WSACleanup();
            system("pause");
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(puerto);
        addr.sin_addr.S_un.S_addr = inet_addr(direccionIp.c_str());

        // Connect to server.
        iResult = connect(ConnectSocket, (SOCKADDR *)&addr, sizeof(addr));
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
        }

        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Error al contectarse al Servidor" << endl;
            WSACleanup();
            system("pause");
        }else{
            conectado = true;
            cout << "Conectado al Servidor!" << endl;
            system("pause");
        }
        

    }
    void Enviar2(char* data)
    {
        strcpy(buffer, data);
        send(ConnectSocket, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        cout << "Mensaje enviado!" << endl;
    }
    void Enviar(char* data)
    {
        strcpy(buffer, data);
        iResult = send(ConnectSocket, buffer, sizeof(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
        if (iResult == SOCKET_ERROR) {
            printf("Error al enviar: %d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
        }else{
            cout << "Mensaje enviado con exito" << endl;
        }
    }

    string Recibir()
    {
        string aux;
        iResult = recv(ConnectSocket, buffer, recvbuflen, 0);
        if (iResult > 0){
                aux = buffer;
                cout << "mensaje recibido:" << endl;
                cout << aux << endl;
                memset(buffer, 0, sizeof(buffer)); 
            }
            else if (iResult == 0)
                printf("La conexion ha sido cerrada\n");
            else
                printf("Fallo en el Recibir(): %d\n", WSAGetLastError());
        
        return aux;
    }

    void CerrarSocket()
    {
       closesocket(ConnectSocket);
       WSACleanup();
       cout << "Socket cerrado." << endl;
    }
};

class AdminTraducciones{
    int eleccionMenu;
    string traduccion;
    Cliente *cliente;

    public:
    AdminTraducciones(Cliente *clienteRecibido){
        cliente = clienteRecibido;
    }
    
    void ObtenerTraduccion(){
        string respuestaServidor;
        cout << "Bienvenido al Obtener Traduccion:" << endl;
        cout<<"Ingrese palabra en ingles a traducir:"<<endl;
        cin.clear();
        getline(std::cin >> std::ws, traduccion);

        cliente->Enviar2(const_cast<char*>(traduccion.c_str()));
        respuestaServidor = cliente->Recibir();
    }

    void NuevaTraduccion(){
        string respuestaServidor;
        cout << "Bienvenido al Ingresar nueva Traduccion:" << endl;
        cout<<"Ingrese palabra en ingles a traducir nueva con su correspondiente traduccion en español."<<endl;
        cout<<"Debera ingresarla con el correspondiente formato => ej: palabraEnInglés:traducciónEnEspañol."<<endl;
        cin.clear();
        getline(std::cin >> std::ws, traduccion);

        cliente->Enviar2(const_cast<char*>(traduccion.c_str()));

        respuestaServidor = cliente->Recibir();
        system("pause");
    }
};

class AdminUsuarios{
    int eleccionMenu;
    string usuario;
    string contra;
    Cliente *cliente;
    bool volverAlMenu = false;

    public:
    AdminUsuarios(Cliente *clienteRecibido){
        cliente = clienteRecibido;
    }
    
    void MenuAltaDesbloqueo(){

        do{
        volverAlMenu = false;
        cout<<"3.1 Alta\n";
        cout<<"3.2 Desbloqueo\n";
        cin.clear();
        cin>>eleccionMenu;
        cliente->Enviar2(const_cast<char*>(to_string(eleccionMenu).c_str()));
        string respuestaServidor;

        switch(eleccionMenu)
        {
            case 1:
                volverAlMenu = AltaUsuario();
                break;
            case 2:
                volverAlMenu = DesbloqueoUsuario();
                break;
        }
        }while(volverAlMenu == true);
    }

    bool AltaUsuario(){
        string respuestaServidor;
        cout << "Bienvenido al Alta." << endl;
        cout<<"Ingrese usuario nuevo:"<<endl;
        cin.clear();
        getline(std::cin >> std::ws, usuario);

        if(usuario.compare("/salir") == 0){
            return true;
        }

        cout<<"Ingrese contraseña nueva:"<<endl;
        cin.clear();
        getline(std::cin >> std::ws, contra);

        if(contra.compare("/salir") == 0){
            return true;
        }

        cliente->Enviar2(const_cast<char*>(usuario.c_str()));
        cliente->Enviar2(const_cast<char*>(contra.c_str()));

        respuestaServidor = cliente->Recibir();
        system("pause");
        return false;
    }


    bool DesbloqueoUsuario(){
        string respuestaServidor;
        cout << "Bienvenido al Desbloqueo." << endl;
        cout << "Lista de usuarios bloqueados:" << endl;
        system("pause");

        respuestaServidor = cliente->Recibir();

        cout<<"Ingrese usuario a desbloquear:"<<endl;
        getline(std::cin >> std::ws, usuario);

        if(usuario.compare("/salir") == 0){
            return true;
        }

        cliente->Enviar2(const_cast<char*>(usuario.c_str()));

        respuestaServidor = cliente->Recibir();
        system("pause");
        return false;
    }
};

int main()
{
    Cliente *cliente;
    AdminUsuarios *adminUsuarios;
    AdminTraducciones *adminTraducciones;
    bool credencialesValidas = false;
    bool isAdmin = false;
    
    do{
        int puerto;
        string r;
        // cout<<"Ingrese IP:"<<endl;
        // getline(std::cin, r);
        // cout<<"Ingrese Puerto:"<<endl;
        // cin.clear();
        // cin >> puerto;
        //comentarinet_addr("192.168.1.41")
        r = "127.168.1.41";
        puerto = 5555;
        
        //pido usuario y contraseña
        string usuario;
        string contra;
        cout<<"Ingrese usuario:"<<endl;
        getline(std::cin, usuario);
        cout<<"Ingrese contraseña:"<<endl;
        getline(std::cin, contra);
                cin.clear();

        cliente = new Cliente(r, puerto);
                        system("pause");

                    cout << "envio user y contra"<< endl;

        cliente->Enviar2(const_cast<char*>(usuario.c_str()));
        cliente->Enviar2(const_cast<char*>(contra.c_str()));

        string mensajeValidacionCred = cliente->Recibir();
        if(mensajeValidacionCred.compare("Datos de usuario incorrectos") == true){
            cliente->CerrarSocket();
            cout<<"Datos de usuario incorrectos. Ingrese datos de nuevo. "<<endl;
        }else{
            credencialesValidas = true;
            string rol = cliente->Recibir();
            if(rol.compare("admin")){
                isAdmin = true;
            }else if(rol.compare("consulta")){
                isAdmin = false;
            }
            system("pause");

        }
    }while(credencialesValidas == false);

    int eleccionMenu;
    
    if(cliente->conectado){
        do{
            adminUsuarios = new AdminUsuarios(cliente); 
            adminTraducciones = new AdminTraducciones(cliente);
            
            cout<<"1. Traducir\n";
            cout<<"2. Nueva traduccion\n";
            cout<<"3. Usuarios\n";
            cout<<"4. Ver registro de actividades\n";
            cout<<"5. Cerrar sesión\n";
            cout<<"Ingresar eleccion(1-3): ";
            // no me agarra el cin si los cin anteriores no son del tipo pedido, hay q validar el de puerto
            cin.clear();
            cin>>eleccionMenu;      
            string x = to_string(eleccionMenu);
            cliente->Enviar2(const_cast<char*>(x.c_str()));
            cout << x;
            system("pause");

            switch(eleccionMenu)
            {
                case 1:
                    adminTraducciones->ObtenerTraduccion();
                    break;
                case 2:
                    adminTraducciones->NuevaTraduccion();
                    break;
                case 3:
                    adminUsuarios->MenuAltaDesbloqueo();
                    break;
                case 4:
                    cliente->Recibir();
                    break;
                case 5:
                    cliente->CerrarSocket();
                    cout << "Sesion cerrada" << endl;
                    system("pause");
                    break;
                default:
                    cout<<"\nEleccion inexistente!";
                    break;
            }
            cin.clear();
            cout<<"\n------------------------\n";

        }while(true);
    }
    

    return 0;


}
