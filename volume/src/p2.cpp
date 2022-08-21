#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

typedef struct {
  char codigo[5];
  char nombre[11];
  char apellidos[20];
  char carrera[15];

  int ciclo;
  float mensualidad;
} Alumno;
typedef struct {
  Alumno data;
  int lastRemoved;
  bool deleted;
} Record;

class Database {
  static const size_t recordSize = sizeof(Record);
  static const size_t dataSize = sizeof(Alumno);
  static const std::fstream::openmode mode =
      std::fstream::binary | std::fstream::in | std::fstream::out;

  std::string filename;

 public:
  Database(std::string filename_) : filename(filename_) {
    std::fstream file(filename, std::fstream::binary | std::fstream::out);
    if (!file.good()) throw std::runtime_error("error oppening the db\n");
    Record dummiRecord = {{}, -1, true};
    file.write((char*)&dummiRecord, recordSize);
    if (!file.good()) throw std::runtime_error("error writing dummi record\n");
    file.flush();
    file.close();
  }
  ~Database() {}

  std::vector<Alumno> load() {
    std::ifstream file(filename, std::fstream::binary);
    if (!file.good()) throw std::runtime_error("error oppening the db\n");
    std::string content(std::istreambuf_iterator<char>(file), {});
    if (!file.good()) throw std::runtime_error("error reading db content\n");
    file.close();
    std::size_t count = content.size() / recordSize;
    std::cout << count << "\n";
    std::vector<Alumno> records(count);
    Record temp = {};
    for (std::size_t i = 0; i < count; i++) {
      std::memcpy(&temp, &content[i * recordSize], recordSize);
      records[i] = temp.data;
    }
    return records;
  }
  void add(Alumno alum) {
    std::fstream file(filename, mode);
    if (!file.good()) throw std::runtime_error("error oppening the db\n");
    Record tmp = {};
    Record record = {alum, -1, false};
    // check for removed record
    file.read((char*)&tmp, recordSize);
    if (!file.good()) throw std::runtime_error("error reading meta record\n");
    int last = tmp.lastRemoved;
    std::cout << last << std::endl;
    if (last >= 0) {
      // override
      file.seekg(last * recordSize);
      file.read((char*)&tmp, recordSize);
      if (!file.good()) throw std::runtime_error("error reading last record\n");
      file.seekg(last * recordSize);
      file.write((char*)&record, recordSize);
      if (!file.good())
        throw std::runtime_error("error writing replacing new record\n");
      file.seekg(std::ios::beg);
      file.write((char*)&tmp, recordSize);
      if (!file.good())
        throw std::runtime_error("error writing replacing meta record\n");
    } else {
      // append
      file.seekg(std::ios::end);
      file.write((char*)&record, recordSize);
      if (!file.good())
        throw std::runtime_error("error appending new record\n");
    }
    file.flush();
    file.close();
  }
  Alumno readRecord(int pos) {
    Record record = {};
    std::fstream file(filename, mode);
    file.seekg(pos * recordSize);
    // check if erased
    while (file.good()) {
      file.read((char*)&record, recordSize);
      if (!file.good()) throw std::runtime_error("error reading pos record\n");
      if (record.lastRemoved >= 0) {
        return record.data;
      }
    }
    return {};
  }

  bool remove(int pos) {
    Record record = {};
    Record tmp = {{}, pos, true};
    std::fstream file(filename, mode);
    // read list head
    file.read((char*)&record, recordSize);
    // update head
    file.seekg(0);
    file.write((char*)&tmp, recordSize);
    // write past head in the pos record
    file.seekg(pos * recordSize);
    if (file.good()) {
      file.write((char*)&record, recordSize);
      return true;
    }
    return false;
  }
};

std::ostream& operator<<(std::ostream& os, Alumno alum) {
  os << "{ ";
  os << alum.codigo << ", ";
  os << alum.nombre << ", ";
  os << alum.apellidos << ", ";
  os << alum.carrera << ", ";
  os << alum.ciclo << ", ";
  os << alum.mensualidad << "}\n";
  return os;
}

int p2Test() {
  Database db("/data/testp2");
  Alumno ta = {"codi", "nombre", "apellido", "carrera", 1, 23};
  std::cout << ta;
  int errors = 0;
  // add
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  // load
  auto vec = db.load();
  for (int i = 0; i < vec.size(); i++) std::cout << vec[i];
  return errors;
}