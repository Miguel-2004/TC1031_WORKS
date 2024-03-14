#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

// Definición de la clase Ticket para representar la información de un ticket de estacionamiento
class Ticket {
private:
    std::string matricula;
    time_t hora_llegada;
    int planta;
    int plaza;
public:
    Ticket(const std::string &matricula, int planta, int plaza) : matricula(matricula), planta(planta), plaza(plaza) {
        hora_llegada = std::time(nullptr);
    }

    const std::string &getMatricula() const {
        return matricula;
    }

    time_t getHoraLlegada() const {
        return hora_llegada;
    }

    int getPlanta() const {
        return planta;
    }

    int getPlaza() const {
        return plaza;
    }
};

// Definición de la clase Plaza para representar una plaza de estacionamiento
class Plaza {
private:
    bool ocupada;
    std::string matricula;
public:
    Plaza() : ocupada(false), matricula("") {}

    bool estaOcupada() const {
        return ocupada;
    }

    const std::string &getMatricula() const {
        return matricula;
    }

    void aparcar(const std::string &matricula) {
        ocupada = true;
        this->matricula = matricula;
    }

    void desaparcar() {
        ocupada = false;
        matricula = "";
    }
};

// Definición de la clase Planta para representar una planta del estacionamiento
class Planta {
private:
    std::vector<Plaza> plazas;
public:
    Planta(int num_plazas) {
        plazas.resize(num_plazas);
    }

    int numPlazas() const {
        return plazas.size();
    }

    bool hayPlazaLibre() const {
        for (const Plaza &plaza : plazas) {
            if (!plaza.estaOcupada()) {
                return true;
            }
        }
        return false;
    }

    int buscarPlazaLibre() const {
        for (int i = 0; i < plazas.size(); ++i) {
            if (!plazas[i].estaOcupada()) {
                return i + 1; // Sumamos 1 para obtener el número de plaza real (1-indexed)
            }
        }
        return -1; // Si no hay plaza libre, retorna -1
    }

    bool aparcarVehiculo(const std::string &matricula, int &plaza) {
        if (hayPlazaLibre()) {
            plaza = buscarPlazaLibre();
            plazas[plaza - 1].aparcar(matricula); // Restamos 1 para obtener el índice correcto
            return true;
        }
        return false;
    }

    void desaparcarVehiculo(int plaza) {
        plazas[plaza - 1].desaparcar(); // Restamos 1 para obtener el índice correcto
    }

    std::vector<Plaza> getPlazas() const {
        return plazas;
    }
};

// Definición de la clase Ascensor para representar un ascensor
class Ascensor {
private:
    bool ocupado;
    int planta_actual;
public:
    Ascensor() : ocupado(false), planta_actual(1) {}

    bool estaOcupado() const {
        return ocupado;
    }

    int getPlantaActual() const {
        return planta_actual;
    }

    void ocupar() {
        ocupado = true;
    }

    void desocupar() {
        ocupado = false;
    }

    void irAPlanta(int planta) {
        planta_actual = planta;
    }
};

// Definición de la clase Estacionamiento para representar el estacionamiento completo
class Estacionamiento {
private:
    std::vector<Planta> plantas;
    std::vector<Ascensor> ascensores;
    std::vector<Ticket> tickets;
public:
    Estacionamiento(int num_plantas, int num_plazas_por_planta, int num_ascensores) {
        plantas.resize(num_plantas, Planta(num_plazas_por_planta));
        ascensores.resize(num_ascensores);
    }

    bool hayPlazaLibre() const {
        for (const Planta &planta : plantas) {
            if (planta.hayPlazaLibre()) {
                return true;
            }
        }
        return false;
    }

    bool aparcarVehiculo(const std::string &matricula, int &planta, int &plaza, int &ascensor) {
        if (hayPlazaLibre()) {
            for (int i = 0; i < ascensores.size(); ++i) {
                if (!ascensores[i].estaOcupado()) {
                    ascensor = i + 1;
                    ascensores[i].ocupar();
                    ascensores[i].irAPlanta(0); // Ir a la planta de entrada
                    break;
                }
            }
            for (int i = 0; i < plantas.size(); ++i) {
                if (plantas[i].hayPlazaLibre()) {
                    plantas[i].aparcarVehiculo(matricula, plaza);
                    planta = i + 1;
                    tickets.emplace_back(matricula, planta, plaza);
                    return true;
                }
            }
        }
        return false;
    }

    bool desaparcarVehiculo(const Ticket &ticket, int &ascensor) {
        for (int i = 0; i < ascensores.size(); ++i) {
            if (!ascensores[i].estaOcupado()) {
                ascensor = i + 1;
                ascensores[i].ocupar();
                ascensores[i].irAPlanta(ticket.getPlanta());
                break;
            }
        }
        plantas[ticket.getPlanta() - 1].desaparcarVehiculo(ticket.getPlaza());
        ascensores[ascensor - 1].desocupar();
        return true;
    }

    void mostrarPlazasOcupadas() const {
        for (const Planta &planta : plantas) {
            for (int i = 0; i < planta.numPlazas(); ++i) {
                if (planta.getPlazas()[i].estaOcupada()) {
                    std::cout << "Planta " << i + 1 << ", Plaza " << i + 1 << ": Matrícula - " << planta.getPlazas()[i].getMatricula() << std::endl;
                }
            }
        }
    }

    void mostrarReportePlazas() const {
        for (int i = 0; i < plantas.size(); ++i) {
            int plazas_ocupadas = 0;
            for (int j = 0; j < plantas[i].numPlazas(); ++j) {
                if (plantas[i].getPlazas()[j].estaOcupada()) {
                    plazas_ocupadas++;
                }
            }
            std::cout << "Planta " << i + 1 << ": Plazas ocupadas - " << plazas_ocupadas << ", Plazas libres - " << plantas[i].numPlazas() - plazas_ocupadas << std::endl;
        }
    }
};

// Función para probar el estacionamiento con casos de prueba
void ejecutarCasosPrueba() {
    Estacionamiento estacionamiento(3, 15, 5);

    // Casos de prueba para aparcar vehículos
    std::cout << "Casos de prueba para aparcar vehículos:" << std::endl;
    int planta, plaza, ascensor;

    bool resultado = estacionamiento.aparcarVehiculo("ABC123", planta, plaza, ascensor);
    if (resultado) {
        std::cout << "Vehículo ABC123 aparcado en Planta " << planta << ", Plaza " << plaza << ", Ascensor " << ascensor << std::endl;
    } else {
        std::cout << "No hay plaza disponible para aparcar el vehículo ABC123." << std::endl;
    }

    resultado = estacionamiento.aparcarVehiculo("DEF456", planta, plaza, ascensor);
    if (resultado) {
        std::cout << "Vehículo DEF456 aparcado en Planta " << planta << ", Plaza " << plaza << ", Ascensor " << ascensor << std::endl;
    } else {
        std::cout << "No hay plaza disponible para aparcar el vehículo DEF456." << std::endl;
    }

    // Casos de prueba para desaparcar vehículos
    std::cout << "\nCasos de prueba para desaparcar vehículos:" << std::endl;
    Ticket ticket1("ABC123", 1, 1);
    resultado = estacionamiento.desaparcarVehiculo(ticket1, ascensor);
    if (resultado) {
        std::cout << "Vehículo con matrícula ABC123 desaparcado." << std::endl;
    } else {
        std::cout << "No se pudo desaparcar el vehículo con matrícula ABC123." << std::endl;
    }

    Ticket ticket2("DEF456", 2, 3);
    resultado = estacionamiento.desaparcarVehiculo(ticket2, ascensor);
    if (resultado) {
        std::cout << "Vehículo con matrícula DEF456 desaparcado." << std::endl;
    } else {
        std::cout << "No se pudo desaparcar el vehículo con matrícula DEF456." << std::endl;
    }

    // Mostrar listado de plazas ocupadas y reporte de plazas
    std::cout << "\nListado de plazas ocupadas:" << std::endl;
    estacionamiento.mostrarPlazasOcupadas();

    std::cout << "\nReporte de plazas ocupadas y libres:" << std::endl;
    estacionamiento.mostrarReportePlazas();
}

int main() {
    ejecutarCasosPrueba();
    return 0;
}