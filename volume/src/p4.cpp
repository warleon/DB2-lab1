#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace P4 {

typedef struct {
  int ciclo;
  float mensualidad;
  std::string codigo;
  std::string observaciones;
} Matricula;
std::ostream& operator<<(std::ostream& os, const Matricula& mat) {
  os << "{ ";
  os << mat.ciclo << ", ";
  os << mat.mensualidad << ", ";
  os << mat.codigo << ", ";
  os << mat.observaciones << " }";
  return os;
}
typedef struct {
  int g;
  int size;
  int fieldSize[2];
} Metadata;
std::ostream& operator<<(std::ostream& os, const Metadata& mat) {
  os << "{ ";
  os << mat.g << ", ";
  os << mat.size << ", ";
  os << mat.fieldSize[0] << ", ";
  os << mat.fieldSize[1] << " }";
  return os;
}

class Database {
  static const auto bin = std::fstream::binary;
  static const auto in = std::fstream::in;
  static const auto out = std::fstream::out;
  static const auto app = std::fstream::app;

  std::string filename;
  std::string metaname;

  Matricula parse(Metadata met, char* data) {
    Matricula mat;
    memcpy((char*)&mat, data, sizeof(int) + sizeof(float));
    mat.codigo = std::move(
        std::string(data + offsetof(Matricula, codigo),
                    data + offsetof(Matricula, codigo) + met.fieldSize[0]));
    mat.codigo = std::move(
        std::string(data + offsetof(Matricula, codigo) + met.fieldSize[0],
                    data + offsetof(Matricula, codigo) + met.fieldSize[0] +
                        met.fieldSize[1]));
    return mat;
  }
  char* dump(Metadata met, Matricula data) {
    char* buff = new char[met.size];
    memcpy(buff, &data.ciclo, sizeof(int));
    memcpy(buff + offsetof(Matricula, mensualidad), &data.mensualidad,
           sizeof(float));
    memcpy(buff + offsetof(Matricula, codigo), data.codigo.c_str(),
           met.fieldSize[0]);
    memcpy(buff + offsetof(Matricula, codigo) + met.fieldSize[0],
           data.observaciones.c_str(), met.fieldSize[1]);
    return buff;
  }
  Metadata getSomeMeta(Matricula matri) {
    Metadata meta;
    meta.fieldSize[0] = matri.codigo.size();
    meta.fieldSize[1] = matri.observaciones.size();
    meta.size =
        sizeof(int) + sizeof(float) + meta.fieldSize[0] + meta.fieldSize[1];
    return meta;
  }

 public:
  enum filetype : char { data = 0, meta };

  Database(std::string filename_)
      : filename(filename_), metaname(filename_ + "_meta") {}

  int binSize(filetype type) {
    std::ofstream file;
    switch (type) {
      case filetype::data:
        file.open(filename, bin);
        break;
      case filetype::meta:
        file.open(metaname, bin);
        break;
      default:
        break;
    }
    if (!file.good()) throw std::runtime_error("error oppening file at size\n");
    file.seekp(0, std::ios::end);
    return file.tellp();
  }

  std::vector<Matricula> load() {
    std::ifstream file(filename, bin);
    if (!file.good())
      throw std::runtime_error("error oppening datafile at load\n");
    std::ifstream metafile(metaname, bin);
    if (!metafile.good())
      throw std::runtime_error("error oppening metafile at load\n");
    int n = binSize(filetype::meta) / sizeof(Metadata);
    std::vector<Matricula> records(n);
    Metadata meta = {};
    Matricula mat = {};
    for (int pos = 0; pos < n; pos++) {
      metafile.seekg(pos * sizeof(Metadata));
      metafile.read((char*)&meta, sizeof(Metadata));
      char* data = new char[meta.size];
      file.read(data, meta.size);
      mat = parse(meta, data);
      delete[] data;
      data = nullptr;
      records[pos] = mat;
    }
    return records;
  }

  void add(Matricula record) {
    std::ofstream file(filename, bin | std::ios::app | std::ios::ate);
    // file.seekp(0, std::ios::end);
    if (!file.good())
      throw std::runtime_error("error oppening datafile at add\n");
    std::ofstream metafile(metaname, bin | std::ios::app | std::ios::ate);
    // metafile.seekp(0, std::ios::end);
    if (!metafile.good())
      throw std::runtime_error("error oppening metafile at add\n");
    Metadata metaRecord = getSomeMeta(record);
    metaRecord.g = (int)file.tellp();
    std::cout << metaRecord << std::endl;
    metafile.write((char*)&metaRecord, sizeof(Metadata));
    if (!metafile.good())
      throw std::runtime_error("error writing metafile at add\n");
    char* data = dump(metaRecord, record);
    std::cout << std::string(data, data + metaRecord.size) << std::endl;
    file.write(data, metaRecord.size);
    if (!file.good())
      throw std::runtime_error("error writing datafile at add\n");
    metafile.close();
    file.close();
    delete[] data;
    data = nullptr;
  }

  Matricula readRecord(int pos) {
    if (pos > binSize(filetype::meta) / (int)sizeof(Metadata) || pos < 0)
      throw std::runtime_error("error pos out of scope at readRecord\n");

    std::ifstream file(filename, bin);
    if (!file.good())
      throw std::runtime_error("error oppening datafile at readRecord\n");
    std::ifstream metafile(metaname, bin);
    if (!metafile.good())
      throw std::runtime_error("error oppening metafile at readRecord\n");

    metafile.seekg(pos * sizeof(Metadata));
    Metadata meta = {};
    metafile.read((char*)&meta, sizeof(Metadata));
    if (!metafile.good())
      throw std::runtime_error("error reading metafile at readRecord\n");
    char* data = new char[meta.size];
    file.read(data, meta.size);
    if (!file.good())
      throw std::runtime_error("error reading datafile at readRecord\n");
    Matricula mat = parse(meta, data);
    delete[] data;
    data = nullptr;
    return mat;
  }
};

int tests() {
  int errors = 0;
  Database db("p4test");
  Matricula ta = {6, 12345, "holaqtalcomoseencuentra",
                  "muybiengraciasporlapreocupacionyusted?"};
  // add
  std::cout << "-------------------------------" << std::endl;
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  db.add(ta);
  // readpos
  std::cout << "-------------------------------" << std::endl;
  std::cout << db.readRecord(4) << std::endl;
  try {
    std::cout << db.readRecord(10) << std::endl;
    errors++;
  } catch (std::runtime_error& e) {
    std::cout << e.what() << std::endl;
  }
  // load
  auto vec = db.load();
  std::cout << "-------------------------------" << std::endl;
  for (size_t i = 0; i < vec.size(); i++) std::cout << vec[i] << std::endl;
  return errors;
}
}  // namespace P4