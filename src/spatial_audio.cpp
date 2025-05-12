#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <sstream>

/////////////////////////////////////////////////////////
//                      Airplane                       //
/////////////////////////////////////////////////////////
// Ekranda yatay eksende hareket eden, sesi Doppler    //
// etkisine göre değişen bir uçak sınıfı.              //
/////////////////////////////////////////////////////////
class Airplane {
public:
    sf::Vector2f position;       // Uçağın pozisyonu (x, y)
    sf::Vector2f velocity;       // Uçağın yön vektörü
    float speed = 100.0f;        // Uçağın hızı (piksel/saniye)

    // Kurucu: başlangıç pozisyonu verilir
    Airplane(float startX, float startY) {
        position = sf::Vector2f(startX, startY);
        velocity = sf::Vector2f(1.0f, 0.0f); // Sabit sağa yönelim
    }

    // Uçak pozisyonunu günceller (deltaTime: geçen süre)
    void update(float deltaTime) {
        position += velocity * speed * deltaTime;

        // Uçak ekran dışına çıkarsa başa sar
        if (position.x > 1000) {
            position.x = -100;
        }
    }
};

/////////////////////////////////////////////////////////
//             Doppler Pitch Hesaplama Fonksiyonu      //
/////////////////////////////////////////////////////////
// Uçağın hızına ve yönüne göre pitch (frekans) ayarlar.//
// Yaklaşıyorsa frekans artar, uzaklaşıyorsa azalır.    //
/////////////////////////////////////////////////////////
float calculateDopplerPitch(sf::Vector2f airplanePos, sf::Vector2f airplaneVelocity, float airplaneSpeed, sf::Vector2f listenerPos) {
    const float speedOfSound = 343.0f; // Havadaki ses hızı (m/s)

    float distance = listenerPos.x - airplanePos.x; // x ekseni baz alınarak mesafe
    float relativeVelocity = airplaneVelocity.x * airplaneSpeed; // Uçağın x yönündeki hızı

    // Uçak yaklaşıyor mu kontrolü (pozitif çarpım → yaklaşma)
    bool isApproaching = (distance * relativeVelocity) > 0;

    float pitch = 1.0f;

    // Doppler formülü uygulanır
    if (relativeVelocity != 0) {
        if (isApproaching) {
            pitch = speedOfSound / (speedOfSound - fabs(relativeVelocity));
        }
        else {
            pitch = speedOfSound / (speedOfSound + fabs(relativeVelocity));
        }
    }

    // Doppler etkisini yumuşatma (fiziksel modeli abartmamak için)
    float dopplerStrength = 0.3f; // 1.0 = tam etki, 0.0 = etkisiz
    float rawPitch = pitch;
    pitch = 1.0f + (rawPitch - 1.0f) * dopplerStrength;

    // Pitch değerini sınırla (kulak dostu aralıkta tut)
    if (pitch < 0.5f) pitch = 0.5f;
    if (pitch > 2.0f) pitch = 2.0f;

    return pitch;
}

/////////////////////////////////////////////////////////
//                    Ana Fonksiyon                    //
/////////////////////////////////////////////////////////
int main() {
    // Pencere oluştur
    sf::RenderWindow window(sf::VideoMode(800, 600), "Doppler Effect Simulation");
    window.setFramerateLimit(60); // Sabit FPS

    // Yazı tipi yükle
    sf::Font font;
    if (!font.loadFromFile("C:/Users/halil/Documents/GitHub/Spatial-Audio/src/x64/Debug/resources/tuffy.ttf")) {
        return EXIT_FAILURE;
    }

    // Bilgi metni ayarları
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::Black);
    infoText.setPosition(10, 10);

    // Uçak nesnesi oluştur
    Airplane airplane(-100, 300); // Ekranın sol dışından başlar

    // Dinleyici sabit konumda (ekranın ortası)
    sf::Vector2f listenerPosition(400, 300);

    // Uçak sesi yükle
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("C:/Users/halil/Documents/GitHub/Spatial-Audio/src/x64/Debug/resources/aygaz_music.ogg")) {
        return EXIT_FAILURE;
    }

    // Ses nesnesini başlat
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();

    ///////////////////////////////////////////////////////
    //                  Slider Arayüzü                   //
    ///////////////////////////////////////////////////////
    // Hız kontrol slider'ı (görsel geri bildirim sağlar)
    sf::RectangleShape sliderBar(sf::Vector2f(200, 10));
    sliderBar.setFillColor(sf::Color::Black);
    sliderBar.setPosition(300, 500);

    sf::CircleShape sliderThumb(10);
    sliderThumb.setFillColor(sf::Color::Green);
    sliderThumb.setPosition(airplane.speed * 2.0f + 300 - 10, 495);

    ///////////////////////////////////////////////////////
    //                     Oyun Döngüsü                  //
    ///////////////////////////////////////////////////////
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Slider'a tıklama ve hareket
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

        // Slider pozisyonuna göre uçağın hızını ayarla
        airplane.speed = (sliderThumb.getPosition().x - 300) / 2.0f;

        // Sabit zaman dilimiyle güncelle (60 FPS)
        float deltaTime = 1.0f / 60.0f;
        airplane.update(deltaTime);

        // Dinleyici ile uçak arasındaki mesafeyi hesapla
        float distance = std::sqrt(
            std::pow(airplane.position.x - listenerPosition.x, 2) +
            std::pow(airplane.position.y - listenerPosition.y, 2)
        );

        // Mesafeye göre ses seviyesi ayarla (doğrusal azalma)
        float volume = 100.0f - (distance / 5.0f);
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;
        sound.setVolume(volume);

        // Doppler etkisine göre pitch ayarla
        float pitch = calculateDopplerPitch(airplane.position, airplane.velocity, airplane.speed, listenerPosition);
        sound.setPitch(pitch);

        // Bilgi metnini güncelle
        std::stringstream ss;
        ss << "Distance: " << (int)distance << " pixels\n";
        ss << "Volume: " << (int)volume << "%\n";
        ss << "Pitch: " << pitch << "\n";
        ss << "Speed: " << (int)airplane.speed;
        infoText.setString(ss.str());

        // Ekranı temizle ve tüm görsel elemanları çiz
        window.clear(sf::Color::White);

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

        // Slider çizimi
        window.draw(sliderBar);
        window.draw(sliderThumb);

        // Bilgi metni
        window.draw(infoText);

        // Çizimi ekrana aktar
        window.display();
    }

    return EXIT_SUCCESS;
}