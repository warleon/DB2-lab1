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
  static const std::fstream::openmode mode = std::fstream::binary;

  std::string filename;

 public:
  Database(std::string filename_) : filename(filename_) {
    std::ofstream file(filename, mode);
    if (!file.good())
      throw std::runtime_error("error oppening the db at constructor\n");
    Record dummiRecord = {{}, -1, true};
    file.write((char*)&dummiRecord, recordSize);
    if (!file.good()) throw std::runtime_error("error writing dummi record\n");
    file.flush();
    file.close();
  }
  ~Database() {}

  std::vector<Alumno> load() {
    std::ifstream file(filename, mode);
    if (!file.good())
      throw std::runtime_error("error oppening the db at load\n");
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
    std::fstream file(filename, mode | std::fstream::in | std::fstream::out);
    if (!file.good())
      throw std::runtime_error("error oppening the db at add\n");
    Record tmp = {};
    Record record = {alum, -1, false};
    // check for removed record
    file.read((char*)&tmp, recordSize);
    if (!file.good()) throw std::runtime_error("error reading meta record\n");
    int last = tmp.lastRemoved;
    if (last > 0) {
      // override
      file.seekg(last * recordSize);
      file.read((char*)&tmp, recordSize);
      if (!file.good()) throw std::runtime_error("error reading last record\n");
      file.seekp(last * recordSize);
      file.write((char*)&record, recordSize);
      if (!file.good())
        throw std::runtime_error("error writing replacing new record\n");
      file.seekp(0, std::ios::beg);
      file.write((char*)&tmp, recordSize);
      if (!file.good())
        throw std::runtime_error("error writing replacing meta record\n");
    } else {
      // append
      file.seekp(0, std::ios::end);
      file.write((char*)&record, recordSize);
      if (!file.good())
        throw std::runtime_error("error appending new record\n");
    }
    file.flush();
    file.close();
  }

  int size() {
    std::ifstream file(filename, mode);
    if (!file.good())
      throw std::runtime_error("error opening the file at size\n");
    const auto begin = file.tellg();
    file.seekg(0, std::ios::end);
    const auto end = file.tellg();
    const auto fsize = (end - begin);
    return fsize / recordSize;
  }

  Alumno readRecord(int pos) {
    Record record = {};
    std::ifstream file(filename, mode);
    if (!file.good())
      throw std::runtime_error("error opening the file at readRecord\n");
    if (pos > size() || pos < 1)
      throw std::runtime_error("error pos out of scope\n");
    file.seekg(pos * recordSize);
    // check if erased
    while (file.good()) {
      file.read((char*)&record, recordSize);
      if (!file.good()) throw std::runtime_error("error reading pos record\n");
      if (!record.deleted) {
        return record.data;
      }
    }
    return {};
  }

  bool remove(int pos) {
    if (pos > size() || pos < 1) return false;
    Record record = {};
    Record tmp = {{}, pos, true};
    std::fstream file(filename, mode | std::fstream::in | std::fstream::out);
    if (!file.good())
      throw std::runtime_error("error opening the file at remove\n");
    // read list head
    file.read((char*)&record, recordSize);
    if (!file.good()) throw std::runtime_error("error reading dummi record\n");
    // update head
    file.seekp(0, std::ios::beg);
    file.write((char*)&tmp, recordSize);
    if (!file.good()) throw std::runtime_error("error writing dummi record\n");
    // write past head in the pos record
    file.seekp(pos * recordSize);
    file.write((char*)&record, recordSize);
    if (!file.good())
      throw std::runtime_error("error writing removed head record\n");
    return true;
  }
};

std::ostream& operator<<(std::ostream& os, Alumno alum) {
  os << "{ ";
  os << alum.codigo << ", ";
  os << alum.nombre << ", ";
  os << alum.apellidos << ", ";
  os << alum.carrera << ", ";
  os << alum.ciclo << ", ";
  os << alum.mensualidad << " }";
  return os;
}

int p2Test() {
  Database db("/data/testp2");
  Alumno ta = {"2222", "nombre", "apellido", "carrera", 1, 23};
  std::cout << ta << std::endl;
  int errors = 0;
  // add
  std::cout << "-------------------------------" << std::endl;
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  // load
  auto vec = db.load();
  std::cout << "-------------------------------" << std::endl;
  for (int i = 0; i < vec.size(); i++) std::cout << vec[i] << std::endl;
  // readpos
  std::cout << "-------------------------------" << std::endl;
  std::cout << db.readRecord(4) << std::endl;
  try {
    std::cout << db.readRecord(10) << std::endl;
  } catch (std::runtime_error e) {
    std::cout << e.what() << std::endl;
  }
  std::cout << "-------------------------------" << std::endl;
  // remove
  if (db.remove(10)) errors++;
  if (!db.remove(3)) errors++;
  if (!db.remove(4)) errors++;
  // load
  vec = db.load();
  std::cout << "-------------------------------" << std::endl;
  for (int i = 0; i < vec.size(); i++) std::cout << vec[i] << std::endl;
  // readpos
  std::cout << "-------------------------------" << std::endl;
  std::cout << db.readRecord(4) << std::endl;
  // add
  std::cout << "-------------------------------" << std::endl;
  db.add(ta);
  vec = db.load();
  std::cout << "-------------------------------" << std::endl;
  for (int i = 0; i < vec.size(); i++) std::cout << vec[i] << std::endl;
  return errors;
}