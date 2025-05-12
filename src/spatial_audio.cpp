#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <sstream>

/////////////////////////////////////////////////////////
//                      Airplane                       //
/////////////////////////////////////////////////////////
// Uçak sınıfı: Ekranda yatay eksende hareket eden     //
// ve sesi Doppler etkisine göre ayarlanan bir nesne. //
/////////////////////////////////////////////////////////
class Airplane {
public:
    sf::Vector2f position;       // Uçağın pozisyonu (x, y)
    sf::Vector2f velocity;       // Yön vektörü (normalleştirilmiş)
    float speed = 100.0f;        // Uçağın hızı (piksel/saniye)

    // Kurucu: başlangıç pozisyonu verilir
    Airplane(float startX, float startY) {
        position = sf::Vector2f(startX, startY);
        velocity = sf::Vector2f(1.0f, 0.0f); // Sağ yöne doğru birim vektör
    }

    // Pozisyonu güncelle (deltaTime'e göre ilerleme)
    void update(float deltaTime) {
        position += velocity * speed * deltaTime;

        // Ekranı geçtiğinde başa dön (loop etkisi)
        if (position.x > 1000) {
            position.x = -100;
        }
    }
};

/////////////////////////////////////////////////////////
//             Doppler Pitch Hesaplama Fonksiyonu      //
/////////////////////////////////////////////////////////
// Sesin pitch değerini Doppler etkisine göre hesaplar.//
// Sesin frekansı yaklaşırken artar, uzaklaşırken azalır//
/////////////////////////////////////////////////////////
float calculateDopplerPitch(sf::Vector2f airplanePos, sf::Vector2f airplaneVelocity, float airplaneSpeed, sf::Vector2f listenerPos) {
    const float speedOfSound = 343.0f; // Havanın içindeki ses hızı (m/s olarak varsayıyoruz)

    float distance = listenerPos.x - airplanePos.x; // x eksenindeki mesafe farkı
    float relativeVelocity = airplaneVelocity.x * airplaneSpeed; // Uçağın x yönündeki hız bileşeni

    // Uçak yaklaşırken distance ve velocity çarpımı pozitif olur
    bool isApproaching = (distance * relativeVelocity) > 0;

    float pitch = 1.0f;

    // Pitch değişimini hesapla (yaklaşıyorsa artar, uzaklaşıyorsa azalır)
    if (relativeVelocity != 0) {
        if (isApproaching) {
            pitch = speedOfSound / (speedOfSound - fabs(relativeVelocity));
        }
        else {
            pitch = speedOfSound / (speedOfSound + fabs(relativeVelocity));
        }
    }

    // Doppler etkisini yumuşatma faktörü (tam etki istemiyorsak)
    float dopplerStrength = 0.3f; // 0 = etki yok, 1 = tam fiziksel model
    float rawPitch = pitch;
    pitch = 1.0f + (rawPitch - 1.0f) * dopplerStrength;

    // Pitch değerini sınırla (duyulabilir aralık)
    if (pitch < 0.5f) pitch = 0.5f;
    if (pitch > 2.0f) pitch = 2.0f;

    return pitch;
}

/////////////////////////////////////////////////////////
//                    Ana Fonksiyon                    //
/////////////////////////////////////////////////////////
int main() {
    // Pencere oluştur (800x600 boyutunda)
    sf::RenderWindow window(sf::VideoMode(800, 600), "Doppler Effect Simulation");
    window.setFramerateLimit(60); // Sabit FPS ile çalıştır

    // Yazı tipi yükle (hata olursa çık)
    sf::Font font;
    if (!font.loadFromFile("C:/Users/halil/Documents/GitHub/Spatial-Audio/src/x64/Debug/resources/tuffy.ttf")) {
        return EXIT_FAILURE;
    }

    // Ekrandaki bilgi metni ayarları
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::Black);
    infoText.setPosition(10, 10);

    // Uçak nesnesi oluştur
    Airplane airplane(-100, 300); // Ekranın sol dışından başlat

    // Dinleyici ekranın ortasında sabit durur
    sf::Vector2f listenerPosition(400, 300);

    // Uçak sesini yükle (hata olursa çık)
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("C:/Users/halil/Documents/GitHub/Spatial-Audio/src/x64/Debug/resources/aygaz_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Ses nesnesini başlat ve döngüye sok
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();

    ///////////////////////////////////////////////////////
    //                  Slider Arayüzü                   //
    ///////////////////////////////////////////////////////
    // Hızı kontrol eden bir kaydırıcı bar ve başlık
    sf::RectangleShape sliderBar(sf::Vector2f(200, 10));
    sliderBar.setFillColor(sf::Color::Black);
    sliderBar.setPosition(300, 500); // Bar ekranın altına yerleştirilmiş

    sf::CircleShape sliderThumb(10);
    sliderThumb.setFillColor(sf::Color::Green);
    sliderThumb.setPosition(airplane.speed * 2.0f + 300 - 10, 495); // Başlangıç pozisyonu (slider'a göre)

    ///////////////////////////////////////////////////////
    //                  Oyun Döngüsü                      //
    ///////////////////////////////////////////////////////
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Slider başlığına tıklama ve hareket etme işlemleri
            if (event.type == sf::Event::MouseButtonPressed) {
                if (sliderThumb.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    sliderThumb.setPosition(event.mouseButton.x - sliderThumb.getRadius(), 495);
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                if (event.mouseMove.x >= 300 && event.mouseMove.x <= 500) {
                    sliderThumb.setPosition(event.mouseMove.x - sliderThumb.getRadius(), 495);
                }
            }
        }

        ///////////////////////////////////////////////////////
        //           Fiziksel Hesaplamalar ve Güncellemeler  //
        ///////////////////////////////////////////////////////

        // Slider pozisyonuna göre uçağın hızını güncelle
        airplane.speed = (sliderThumb.getPosition().x - 300) / 2.0f;

        // DeltaTime sabit (60 FPS varsayımı)
        float deltaTime = 1.0f / 60.0f;
        airplane.update(deltaTime);

        // Dinleyici ile uçak arasındaki mesafeyi hesapla (2D mesafe)
        float distance = std::sqrt(
            std::pow(airplane.position.x - listenerPosition.x, 2) +
            std::pow(airplane.position.y - listenerPosition.y, 2)
        );

        // Uzaklığa göre ses seviyesi (logaritmik değil, doğrusal)
        float volume = 100.0f - (distance / 5.0f);
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;
        sound.setVolume(volume);

        // Doppler pitch hesapla ve uygula
        float pitch = calculateDopplerPitch(airplane.position, airplane.velocity, airplane.speed, listenerPosition);
        sound.setPitch(pitch);

        // Bilgi metnini güncelle
        std::stringstream ss;
        ss << "Distance: " << (int)distance << " pixels\n";
        ss << "Volume: " << (int)volume << "%\n";
        ss << "Pitch: " << pitch << "\n";
        ss << "Speed: " << (int)airplane.speed;
        infoText.setString(ss.str());

        ///////////////////////////////////////////////////////
        //                  Ekran Çizimleri                  //
        ///////////////////////////////////////////////////////
        window.clear(sf::Color::White); // Arka planı beyaz yap

        // Uçak (kırmızı daire)
        sf::CircleShape airplaneShape(10);
        airplaneShape.setFillColor(sf::Color::Red);
        airplaneShape.setPosition(airplane.position);
        window.draw(airplaneShape);

        // Dinleyici (mavi küçük daire)
        sf::CircleShape listenerShape(5);
        listenerShape.setFillColor(sf::Color::Blue);
        listenerShape.setPosition(listenerPosition);
        window.draw(listenerShape);

        // Slider bileşenleri
        window.draw(sliderBar);
        window.draw(sliderThumb);

        // Bilgi metni
        window.draw(infoText);

        // Ekranı güncelle
        window.display();
    }

    return EXIT_SUCCESS;
}
