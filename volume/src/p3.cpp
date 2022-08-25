#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <climits>

using namespace std;

struct Alumno
{
    string Nombre;
    string Apellidos;
    string Carrera;
    float mensualidad;

    void print()
    {
        cout << Nombre << endl
             << Apellidos << endl
             << Carrera << endl
             << mensualidad << endl;
    }
};

ostream &operator<<(ostream &stream, Alumno &p)
{
    cout << "Begin \n";
    stream << p.Nombre << '|';
    stream << p.Apellidos << '|';
    stream << p.Carrera << '|';
    stream << to_string(p.mensualidad);
    stream << "\n";
    stream << flush;
    return stream;
}

class VariableRecord
{
private:
    string filename;

public:
    VariableRecord(string filename)
    {
        this->filename = filename;
    }

    vector<Alumno> load()
    {
        ifstream File(this->filename, ios::in);
        vector<Alumno> alumnos;
        if (!File.is_open())
            cout << "No se pudo abrir el archivo. \n";

        string line;

        getline(File, line);

        while (getline(File, line))
        {
            char *data = const_cast<char *>(line.c_str());
            data = strtok(data, "|");
            Alumno record;

            record.Nombre = data;
            data = strtok(NULL, "|");

            record.Apellidos = data;
            data = strtok(NULL, "|");

            record.Carrera = data;
            data = strtok(NULL, "|");

            record.mensualidad = atof(data);
            data = strtok(NULL, "|");

            alumnos.push_back(record);
        }

        return alumnos;
    }
    void add(Alumno record)
    {
        ofstream File(this->filename, ios::app | ios::out);
        if (!File.is_open())
            cout << "No se pudo abrir el archivo. \n";

        File << record;

        File.close();
    }

    Alumno readRecord(int pos)
    {
        ifstream File(this->filename, ios::app | ios::in);
        if (!File.is_open())
            cout << "No se pudo abrir el archivo. \n";

        string line;

        getline(File, line);

        for (int i = 0; i < pos; ++i)
            getline(File, line);

        char *data = const_cast<char *>(line.c_str());
        data = strtok(data, "|");
        Alumno record;

        record.Nombre = data;
        data = strtok(NULL, "|");

        record.Apellidos = data;
        data = strtok(NULL, "|");

        record.Carrera = data;
        data = strtok(NULL, "|");

        record.mensualidad = atoi(data);
        data = strtok(NULL, "|");

        return record;
    }
};

int main()
{
    VariableRecord file("datos3.txt");
    Alumno alumno1;
    alumno1 = file.readRecord(1);
    alumno1.print();
    return 0;
}

int p3Test() {
  int errors = 0;
  return errors;
}
