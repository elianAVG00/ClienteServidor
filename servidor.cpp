#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>  
#include <vector>
#include <unistd.h>
#include <sstream>
#include <iterator>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

#define DEFAULT_BUFLEN 512

string PonerFecha() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);
 
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%m-%d-%Y %X", now);
    return buffer;
}

class ArchivoLog{
    public:
        string nombreArchivo = "server.log";
        ofstream archivoLog;

    void CrearArchivo(){
        archivoLog.open(nombreArchivo.c_str(), fstream::out);  
        archivoLog.close();
    }
        
    void EscribirArchivo(vector<string> logueoVector){
        archivoLog.open(nombreArchivo.c_str(), fstream::out);

        for (size_t i = 0; i < logueoVector.size(); i++)
        {
            archivoLog << logueoVector[i] << endl;
        }
        archivoLog.close();
    }

    void EnviarArchivo(){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        while (getline(archivo, linea)) {
            cout << linea << endl;
            const char * c = linea.c_str();
        }
        archivo.close();
    }

    string FormatearLogueo(vector<string> vec){
        stringstream ss;
    
        for (size_t i = 0; i < vec.size(); i++) {
            if (i != 0) {
                ss << "\n";
            }
            ss << vec[i];
        }
    
        return ss.str();  
    }
};

class ManejoTraducciones{
    public:
        string nombreArchivo = "traducciones.txt";
        ofstream   archivoCred;
        int lineaEspecifica;

    void CrearArchivo(){
        
        archivoCred.open ("traducciones.txt", fstream::app);

        archivoCred.close();
    }
        
    void EscribirLinea(string linea){
        archivoCred.open(nombreArchivo.c_str(), ios::app); 

        archivoCred << linea << endl;

        archivoCred.close();
    }

    vector<string> SeparoTraduccion(string traduccion){
        vector<string> tokens;
        stringstream ss(traduccion); 
        char delimitador = ':';

        while (getline(ss, traduccion, delimitador)){
            cout << traduccion << endl;
            tokens.push_back(traduccion);
        }

        return tokens;
    }

    string InsertarNuevaTraduccion(string traduccion){
        traduccion = aMinuscula(traduccion);
        vector<string> traduccionSeparada = SeparoTraduccion(traduccion);
        string result = "No fue posible insertar la traducción. El formato de inserción debe ser palabraEnInglés:traducciónEnEspañol";
        
        if(traduccionSeparada.size() == 1){
            return result; 
        }

        if((traduccionSeparada[0].empty() == false ) || (traduccionSeparada[1].empty() == false)){

            if(ValidarPalabraIng(traduccionSeparada[0]) == true){
                result = "Ya existe una traducción para [" + traduccionSeparada[0] + "]: [" + traduccionSeparada[1] + "]";
            }else{
                EscribirLinea(traduccion);
                result = "Nueva traducción insertada correctamente";
            }
        }

        return result;
    }

    string aMinuscula(string cadena) {
        for (int i = 0; i < cadena.length(); i++) cadena[i] = tolower(cadena[i]);
        return cadena;
    }

    bool ValidarPalabraIng(string palabraIng){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        bool palabraExistente = false;

        while (getline(archivo, linea)) {

            vector<string> lineaTraduccionSeparada = SeparoTraduccion(linea);
            
            //comparo la nueva palabra a traducir con las que ya estaban
            if(palabraIng.compare(lineaTraduccionSeparada[0]) == 0){
                palabraExistente = true;
            }

            if (archivo.eof() || (palabraExistente == true)) {
                break;
            }
        }
        
        archivo.close();

        return palabraExistente;
    }

    string ObtenerTraduccion(string palabraIng){
        palabraIng = aMinuscula(palabraIng);
        string traduccionEsp = BuscarTraduccion(palabraIng);
        string respuesta;

        if(traduccionEsp.empty() || (traduccionEsp.compare("")== 0)){
            respuesta = "No fue posible encontrar la traducción para: " + palabraIng;
        }else{
            respuesta = "[" + palabraIng + "]" + " en inglés es [ " + traduccionEsp + "] en español";
        }

        return respuesta;
    }

    string BuscarTraduccion(string palabraIng){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        bool palabraExistente = false;
        string respuesta = "";

        while (getline(archivo, linea)) {

            vector<string> lineaTraduccionSeparada = SeparoTraduccion(linea);
            
            //comparo la nueva palabra a traducir con las que ya estaban
            if(palabraIng.compare(lineaTraduccionSeparada[0]) == 0){
                respuesta = lineaTraduccionSeparada[1];
                palabraExistente = true;
            }

            if (archivo.eof() || (palabraExistente == true)) {
                break;
            }
        }
        
        archivo.close();

        return respuesta;
    }
};

class ArchivoCredenciales{
    public:
        string nombreArchivo = "credenciales.txt";
        ofstream   archivoCred;
        int lineaEspecifica;

    void CrearArchivo(){
        
        archivoCred.open ("credenciales.txt", fstream::app);

        archivoCred.close();
    }
        
    void EscribirLinea(string linea){
        archivoCred.open(nombreArchivo.c_str(), ios::app); 

        archivoCred << linea << endl;

        archivoCred.close();
    }

    void EscribirLineaEspecifica(string lineaNueva, int numeroLinea){
        //string to hold each line for insert into vector
        string linea;
        //vector to hold all lines
        vector<string> lineasArchivo;
        numeroLinea = numeroLinea - 1;
        //open file for reading
        ifstream archivo(nombreArchivo.c_str());
        if (archivo.is_open())
        {
            //read lines and push into vector
            while (getline(archivo, linea))
            {
                lineasArchivo.push_back(linea);
            }
        }

        //adjust line 4 of vector
        lineasArchivo[numeroLinea] = lineaNueva;

        //open file for writing, use ofstream::trunc to erase file and replace with vector
        ofstream newFile(nombreArchivo.c_str(), ofstream::trunc);

        //iterate vector and add lines
        for (int i = 0; i < lineasArchivo.size(); i++) {
            newFile << lineasArchivo[i] << endl;
        }

        //close file
        newFile.close();
    }

    bool ValidoCredenciales(string usuario, string contra){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        int numeroDeLinea = 0;
        bool credencialesValidas = false;

        while (getline(archivo, linea)) {
            numeroDeLinea++;
            cout << linea << endl;
            system("pause");

            vector<string> lineaDataUsuario = SeparoCredencial(linea);
            lineaEspecifica = numeroDeLinea;
            credencialesValidas = ComparoCredenciales(lineaDataUsuario, usuario, contra);
            if (archivo.eof() || (credencialesValidas == true)) {
                break;
            }
        }
        
        archivo.close();
                    cout << "salidasd" << endl;

                    system("pause");

        return credencialesValidas;
    }

    //0usuario 1contra 2rol 3intentos  
    bool ComparoCredenciales(vector<string> linea, string usuario, string contra){
        if(usuario.compare(linea[0]) != 0){
            cout << "Usuario invalido" << endl;
            cout << linea[0] << endl;
            cout << usuario << endl;
            system("pause");
            return false;
        }

        if(contra.compare(linea[1]) != 0){
            cout << "contraseña invalido" << endl;
            cout << linea[1] << endl;
            cout << contra << endl;

            SumoIntentos(linea);
            return false;
        }

        return true;

    }

    vector<string> SeparoCredencial(string linea){
        vector<string> tokens;
        stringstream ss(linea); 
        char delimitador = '|';

        while (getline(ss, linea, delimitador)){
            cout << linea << endl;
            tokens.push_back(linea);
        }
                    system("pause");

        return tokens;
    }

    void SumoIntentos(vector<string> linea){
        int intentos = stoi(linea[3]);
        intentos = intentos+1;
        linea[3] = to_string(intentos);

        EscribirLineaEspecifica(FormatearLinea(linea[0], linea[1], linea[2], linea[3]), lineaEspecifica);
    }

    string AltaUsuario(string usuario, string contra){
        string result;

        if(ValidoUsuario(usuario) == true){
            result = "Error al dar de alta el nuevo usuario: usuario existente";
        }else{

            EscribirLinea(FormatearLinea(usuario, contra, "Admin", "0"));
            result = usuario + " dado de alta correctamente";
        }

        
        return result;
    }

    bool ValidoUsuario(string usuario){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        bool usuarioExistente = false;

        while (getline(archivo, linea)) {
            vector<string> lineaDataUsuario = SeparoCredencial(linea);
            if(usuario.compare(lineaDataUsuario[0]) == 0){
                usuarioExistente = true;
            }
            if (archivo.eof() || (usuarioExistente == true)) {
                break;
            }
        }

        return usuarioExistente;
    }

    string FormatearLinea(string usuario, string contra, string rol, string intentos){
        string lineaFormateada;
        lineaFormateada = usuario + '|' + contra + '|' + rol + '|' + intentos;

        return lineaFormateada;
    }

    string GetUsuariosBloqueados(){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        vector<string> listaUsuarios;

        while (getline(archivo, linea)) {
            vector<string> lineaDataUsuario = SeparoCredencial(linea);
            int intentosLinea = stoi(lineaDataUsuario[3]);

            if(intentosLinea >= 3){
                listaUsuarios.push_back(lineaDataUsuario[0]);
            }

            if (archivo.eof()) {
                break;
            }
        }

        return FormatearBloqueados(listaUsuarios);
    }

    string DesbloquearUsuario(string usuario){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        bool usuarioExistente = false;
        int numeroDeLinea = 0;
        string respuesta;

        while (getline(archivo, linea)) {
            numeroDeLinea++;

            vector<string> lineaDataUsuario = SeparoCredencial(linea);
            if(usuario.compare(lineaDataUsuario[0]) == 0){
                lineaDataUsuario[3] = "0";

                //mando a escibir en la misma linea con el nro Intetntos actualizado a 0
                EscribirLineaEspecifica(FormatearLinea(lineaDataUsuario[0], lineaDataUsuario[1], lineaDataUsuario[2], lineaDataUsuario[3]), numeroDeLinea);

                usuarioExistente = true;
            }
            if (archivo.eof() || (usuarioExistente == true)) {
                break;
            }
        }

        if(usuarioExistente == false){
            respuesta = usuario + "no encontrado";
        }else{
            respuesta = usuario + "desbloqueado correctamente";
        }
        return respuesta;
    }


    string FormatearBloqueados(vector<string> vec){
        stringstream ss;
    
        for (size_t i = 0; i < vec.size(); i++) {
            if (i != 0) {
                ss << "\n";
            }
            ss << vec[i];
        }
    
        return ss.str();  
    }
};

class Servidor{
    public:
        WSADATA WSAData;
        SOCKET servidor; // escucha posibles conexiones entrantes
        SOCKET ClientSocket; // activo por si encuentra conexion
        SOCKADDR_IN direccionServidor, direccionCliente;
        char buffer[1024];
        ArchivoLog *manejoArchivo = new ArchivoLog();
        vector<string> logueoVector;
        char logueoBuffer[1024];
        int iResult = 0;
        bool vuelvoConectar;
        ArchivoCredenciales *manejoCredenciales;


        Servidor(ArchivoCredenciales *manejoCred, int puerto)
        {
            manejoCredenciales = manejoCred;

            WSAStartup(MAKEWORD(2, 0), &WSAData);// inicio el winsock
            
            manejoArchivo-> CrearArchivo();
            logueoVector.push_back(PonerFecha() + ": =======Inicia Servidor==========");

            servidor = socket(AF_INET, SOCK_STREAM, 0);// pongo al socket dominio de internet, conexion orientada a TCP

            direccionServidor.sin_addr.s_addr = INADDR_ANY;// seteo el IP
            direccionServidor.sin_family = AF_INET;
            direccionServidor.sin_port = htons(puerto);// seteo el puerto

            iResult = bind(servidor, (SOCKADDR *)&direccionServidor, sizeof(direccionServidor));
            if (iResult == SOCKET_ERROR) {
                printf("bind failed with error: %d\n", WSAGetLastError());
                system("pause");
                closesocket(servidor);
                WSACleanup();
            }
            listen(servidor, SOMAXCONN);
            
            logueoVector.push_back(PonerFecha() + ": " + "Socket Creado. Puerto de escucha." + to_string(puerto));
            logueoVector.push_back(PonerFecha() + ": " + "Conexion Aceptada.");
            
            cout << "Escuchando posibles conexiones..." << endl;
            SOCKADDR_IN direccionCliente;
            int clientAddrSize = sizeof(direccionCliente);
            ClientSocket = INVALID_SOCKET;

            iResult = ClientSocket = accept(servidor, (SOCKADDR *)&direccionCliente, &clientAddrSize);
            if(iResult ==SOCKET_ERROR )
            {
                vuelvoConectar = false;
                cout << "Cliente no conectado!"<< endl;
                
            }else{
                vuelvoConectar = true;
                logueoVector.push_back(PonerFecha() + ": " + "Cliente conectado.");
                cout << "Cliente conectado!"<< endl;
            }

            manejoArchivo->EscribirArchivo(logueoVector);
            
            if(ValidarUsuario()){
                cout << "Cliente validado!"<< endl;
                Enviar("Datos de usuario correctos");

            }else{
                cout << "Cliente no existe!"<< endl;
                Enviar("Datos de usuario incorrectos");
                Cerrar();
                vuelvoConectar = false;
            }
            system("pause");

        }

        bool ValidarUsuario()
        {
            string usuario = RecibirData2();
            string contra = RecibirData2();
            cout << usuario<< endl;
            cout << contra<< endl;
                        system("pause");

            bool result = manejoCredenciales->ValidoCredenciales(usuario, contra);
            system("pause");
            return result;
        }

        void ConectarServidor(){
            cout << "Escuchando posibles conexiones..." << endl;
            SOCKADDR_IN direccionCliente;
            int clientAddrSize = sizeof(direccionCliente);
            ClientSocket = INVALID_SOCKET;

            iResult = ClientSocket = accept(servidor, (SOCKADDR *)&direccionCliente, &clientAddrSize);
            if(iResult ==SOCKET_ERROR )
            {
                vuelvoConectar = false;
                cout << "Cliente no conectado!"<< endl;
                
            }else{
                vuelvoConectar = true;
                logueoVector.push_back(PonerFecha() + ": " + "Cliente conectado.");
                cout << "Cliente conectado!"<< endl;
            }
        }


        string RecibirData2()
        {
            recv(ClientSocket, buffer, sizeof(buffer), 0);
            string aux = buffer;
            memset(buffer, 0, sizeof(buffer)); 
            return aux;
        }
        string RecibirData()
        {
            iResult = recv(ClientSocket, buffer, sizeof(buffer), 0);
            // if (iResult > 0) {
                string aux = buffer;
                memset(buffer, 0, sizeof(buffer)); 
                WSACleanup();

                        system("pause");

                return aux;
            // } 
            // else if (iResult == 0)
            //     printf("Connection closing...\n");
            // else {
            //     printf("recv failed: %d\n", WSAGetLastError());
            //     system("pause");
            //     closesocket(ClientSocket);
            //     WSACleanup();
            //             system("pause");

            // }
        }
        
        void Enviar(char* data)
        {
            strcpy(buffer, data);
            iResult = send(ClientSocket, buffer, sizeof(buffer), 0);
            if (iResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                system("pause");
            }
            memset(buffer, 0, sizeof(buffer));
            cout << "Mensaje enviado!" << endl;
        }

        void Cerrar()
        {
            closesocket(ClientSocket);
            WSACleanup();
            cout << "Cliente desconectado, socket cerrado." << endl;
            logueoVector.push_back(PonerFecha() + ": " + "Cliente desconectado.");
            manejoArchivo->EscribirArchivo(logueoVector);
        }
};

int main()
{
    int puerto;
    

    cout << "Bienvenido al Servidor." << endl;
    cout<<"Ingrese puerto:"<<endl;
    cin >> puerto;
    cin.clear();

    ArchivoCredenciales *archivoCredenciales = new ArchivoCredenciales();
    archivoCredenciales->CrearArchivo();

    ManejoTraducciones *manejoTraducciones = new ManejoTraducciones();
    manejoTraducciones->CrearArchivo();
    do
    {
        Servidor *servidor = new Servidor(archivoCredenciales, puerto);
        bool salir = servidor->vuelvoConectar;
        do
        {
            
            string a = servidor->RecibirData2();
            char dato = a[0];
            cout << dato << endl;
            system("pause");

            switch(dato)
            {
                case '1':
                    {
                    string palabraIng = servidor->RecibirData2();

                    string respuesta = manejoTraducciones->ObtenerTraduccion(palabraIng);

                    servidor->Enviar(const_cast<char*>(respuesta.c_str()));
                    break;
                    }
                case '2':
                    {
                    string traduccion = servidor->RecibirData2();

                    string respuesta = manejoTraducciones->InsertarNuevaTraduccion(traduccion);

                    servidor->Enviar(const_cast<char*>(respuesta.c_str()));
                    break;
                    }
                case '3':
                    {
                    a = servidor->RecibirData2();
                    char dato = a[0];
                    string respuesta;

                    if(dato == '1'){
                        string usuario = servidor->RecibirData2();
                        string contra = servidor->RecibirData2();
                        if(usuario.empty() || contra.empty()){
                            respuesta = "Error al dar de alta el nuevo usuario: datos incompletos.";
                        }else
                        {
                            respuesta = archivoCredenciales->AltaUsuario(usuario, contra);
                        }
                        
                        servidor->Enviar(const_cast<char*>(respuesta.c_str()));
                        
                    }else if(dato == '2'){
                        string listaBloqueados = archivoCredenciales->GetUsuariosBloqueados();
                        if(listaBloqueados.empty() || listaBloqueados.find_first_not_of(' ') == string::npos){
                            respuesta = "No se encontraron usuarios bloqueados";
                        }else{
                            servidor->Enviar(const_cast<char*>(listaBloqueados.c_str()));
                            //recibo usuario a desbloquear
                            string usuario = servidor->RecibirData2();
                            respuesta = archivoCredenciales->DesbloquearUsuario(usuario);
                        }
                        servidor->Enviar(const_cast<char*>(respuesta.c_str()));

                    }
                    break;
                    }
                case '4':
                    {
                    string logueado = servidor->manejoArchivo->FormatearLogueo(servidor->logueoVector);
                    servidor->Enviar(const_cast<char*>(logueado.c_str()));
                    break;
                    }
                case '5':
                    servidor->Cerrar();
                    system("pause");
                    salir = false;
                    break;
            }
        }while(salir);
    }while(true);
}
