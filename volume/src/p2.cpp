#include <cstring>
#include <fstream>
#include <sstream>
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
    std::fstream file(filename, mode);
    Record dummiRecord = {{}, -1, true};
    file.write((char*)&dummiRecord, recordSize);
    file.close();
  }
  ~Database() {}
  std::vector<Alumno> load() {
    std::fstream file(filename, mode);
    std::stringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();
    file.close();
    std::size_t count = content.size() / recordSize;
    std::vector<Alumno> records(count);
    Record temp = {};
    for (std::size_t i = 1; i < count; i++) {
      std::memcpy(&temp, &content[i * recordSize], recordSize);
      records[i] = temp.data;
    }
    return records;
  }
  void add(Alumno alum) {
    std::fstream file(filename, mode);
    // check for removed record
    Record tmp = {};
    Record record = {alum, -1, false};

    file.read((char*)&tmp, recordSize);
    std::size_t last = tmp.lastRemoved;
    if (last) {
      // override
      file.seekg(last * recordSize);
      file.read((char*)&tmp, recordSize);
      file.seekg(last * recordSize);
      file.write((char*)&record, recordSize);
      file.seekg(0);
      file.write((char*)&tmp, recordSize);
    } else {
      // append
      file.seekg(std::ios::end);
      file.write((char*)&record, recordSize);
    }
    file.close();
  }
  Alumno readRecord(int pos) {
    Record record = {};
    std::fstream file(filename, mode);
    file.seekg(pos * recordSize);
    // check if erased
    while (file.good()) {
      file.read((char*)&record, recordSize);
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

int p2Test() {
  Database db("testp2");
  Alumno ta = {"codi", "nombre", "apellido", "carrera", 1, 23};
  int errors = 0;
  // add
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  // load
  auto students = db.load();
  if (students.size() != 6) errors++;
  // remove
  if (db.remove(8)) errors++;
  if (!db.remove(3)) errors++;
  // read
  if (db.readRecord(9).mensualidad == ta.mensualidad) errors++;
  if (db.readRecord(3).mensualidad != ta.mensualidad) errors++;
  return errors;
}