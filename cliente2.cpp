#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>  
#include <unistd.h>
#include <sstream>
#include <iterator>
#include <bits/stdc++.h>  

using namespace std;

class ArchivoCredenciales{
    public:
        string nombreArchivo = "credenciales.txt";
        ofstream   archivoCred;

    void CrearArchivo(){
        
        archivoCred.open ("credenciales.txt", fstream::app);

        archivoCred.close();
    }
        
    void EscribirArchivo(vector<string> logueoVector){
        archivoCred.open(nombreArchivo.c_str(), fstream::out);

        for (size_t i = 0; i < logueoVector.size(); i++)
        {
            archivoCred << logueoVector[i] << endl;
        }
        archivoCred.close();
    }

    bool LeerArchivo(string usuario, string contra){
        ifstream archivo(nombreArchivo.c_str());
        string linea;
        int numeroDeLinea = 0;
        bool credencialesValidas = false;

        while (getline(archivo, linea) || credencialesValidas) {
            cout << linea << endl;
            system("pause");

            vector<string> lineaDataUsuario = SeparoCredencial(linea);
            credencialesValidas = ComparoCredenciales(lineaDataUsuario, usuario, contra);
        }
        archivo.close();

        return credencialesValidas;
    }

    bool ComparoCredenciales(vector<string> linea, string usuario, string contra){
        if(usuario.compare(linea[0]) == 0){
            cout << "Usuario valido" << endl;
            system("pause");

        }else{
          cout << "Usuario invalido" << endl;
          cout << linea[0] << endl;
                    cout << usuario << endl;

          system("pause");
        }

        if(contra.compare(linea[1]) == 0){
            cout << "contraseña valido" << endl;
            system("pause");
        }else{
          cout << "contraseña invalido" << endl;
                    cout << linea[1] << endl;
                    cout << contra << endl;

          system("pause");
        }


    }
    //Arreglar el separo credencials
    vector<string> SeparoCredencial(string linea){
        vector<string> tokens;
        stringstream ss(linea); 
        char delimitador = '|';

        while (getline(ss, linea, delimitador)){
            cout << linea << endl;
            tokens.push_back(linea);
            system("pause");
        }

        return tokens;
    }

};


int main()
{
  ArchivoCredenciales *archivoCredenciales = new ArchivoCredenciales();
  archivoCredenciales->CrearArchivo();
  archivoCredenciales->LeerArchivo("user", "abc");

  system("pause");
} 