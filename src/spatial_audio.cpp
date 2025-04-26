#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>
#include <sstream>

// Uçak sınıfı
class Airplane {
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed = 100.0f; // Piksel/saniye (hız)

    Airplane(float startX, float startY) {
        position = sf::Vector2f(startX, startY);
        velocity = sf::Vector2f(1.0f, 0.0f); // Başlangıç hızı
    }

    void update(float deltaTime) {
        position += velocity * speed * deltaTime;

        if (position.x > 1000) {
            position.x = -100;
        }
    }
};

float calculateDopplerPitch(sf::Vector2f airplanePos, sf::Vector2f airplaneVelocity, float airplaneSpeed, sf::Vector2f listenerPos) {
    float speedOfSound = 343.0f;

    // Uçağın dinleyiciye göre x eksenindeki farkı
    float distance = listenerPos.x - airplanePos.x;

    // Uçağın yönü * hızı → göreli hız
    float relativeVelocity = airplaneVelocity.x * airplaneSpeed;

    // Yaklaşıyor mu uzaklaşıyor mu kontrol et
    bool isApproaching = (distance * relativeVelocity) > 0;
    // distance ve velocity aynı işaretliyse: yaklaşıyor

    float pitch = 1.0f;

    if (relativeVelocity != 0) {
        if (isApproaching) {
            pitch = speedOfSound / (speedOfSound - fabs(relativeVelocity));
        }
        else {
            pitch = speedOfSound / (speedOfSound + fabs(relativeVelocity));
        }
    }

    // Doppler etkisini yumuşatmak için
    float dopplerStrength = 0.3f; // 0 = hiç etki yok, 1 = tam etki
    float rawPitch = pitch;
    pitch = 1.0f + (rawPitch - 1.0f) * dopplerStrength;

    // Pitch değerini sınırlıyoruz
    if (pitch < 0.5f) pitch = 0.5f;
    if (pitch > 2.0f) pitch = 2.0f;

    return pitch;
}


int main() {
    // Pencere oluştur
    sf::RenderWindow window(sf::VideoMode(800, 600), "Doppler Effect Simulation");
    window.setFramerateLimit(60);

    // Font yükle
    sf::Font font;
    if (!font.loadFromFile("C:\\Users\\halil\\Documents\\My Documents\\Job\\My studies\\C++\\spatial_audio\\x64\\Debug\\resources\\tuffy.ttf")) {
        return EXIT_FAILURE;
    }

    // Bilgi metni
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::Black);
    infoText.setPosition(10, 10);

    // Uçak oluştur
    Airplane airplane(-100, 300);

    // Dinleyici pozisyonu (ekranın ortası)
    sf::Vector2f listenerPosition(400, 300);

    // Uçak sesi yükle
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("C:\\Users\\halil\\Documents\\My Documents\\Job\\My studies\\C++\\spatial_audio\\x64\\Debug\\resources\\aygaz_music.ogg")) {
        return EXIT_FAILURE;
    }

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setLoop(true);
    sound.play();

    // Slider için parametreler
    sf::RectangleShape sliderBar(sf::Vector2f(200, 10)); // Kaydırıcı bar
    sliderBar.setFillColor(sf::Color::Black);
    sliderBar.setPosition(300, 500); // Slider'ın yeri

    sf::CircleShape sliderThumb(10); // Kaydırıcı başlığı
    sliderThumb.setFillColor(sf::Color::Green);
    sliderThumb.setPosition(airplane.speed * 2.0f + 300 - 10, 495); // Başlangıç yeri

    // Ana döngü
    while (window.isOpen()) {
        // Olayları işle
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Slider'ı hareket ettirmek için mouse olaylarını dinle
            if (event.type == sf::Event::MouseButtonPressed) {
                if (sliderThumb.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    // Slider başlığına tıklanmışsa, kaydırıcıyı takip et
                    sliderThumb.setPosition(event.mouseButton.x - sliderThumb.getRadius(), 495);
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                if (event.mouseMove.x >= 300 && event.mouseMove.x <= 500) {
                    // Mouse hareketi ile slider başlığını güncelle
                    sliderThumb.setPosition(event.mouseMove.x - sliderThumb.getRadius(), 495);
                }
            }
        }

        // Uçağın hızını slider'dan alalım
        airplane.speed = (sliderThumb.getPosition().x - 300) / 2.0f; // Slider'daki pozisyona göre hız

        // Uçağı güncelle
        float deltaTime = 1.0f / 60.0f; // 60 FPS varsayımı
        airplane.update(deltaTime);

        // Mesafe hesapla
        float distance = std::sqrt(
            std::pow(airplane.position.x - listenerPosition.x, 2) +
            std::pow(airplane.position.y - listenerPosition.y, 2)
        );

        // Ses seviyesi hesapla
        float volume = 100.0f - (distance / 5.0f);
        if (volume < 0) volume = 0;
        if (volume > 100) volume = 100;
        sound.setVolume(volume);

        // Doppler efekti için pitch hesapla
        float pitch = calculateDopplerPitch(airplane.position, airplane.velocity, airplane.speed, listenerPosition);
        sound.setPitch(pitch);


        // Bilgi metnini güncelle
        std::stringstream ss;
        ss << "Distance: " << (int)distance << " pixels\n";
        ss << "Volume: " << (int)volume << "%\n";
        ss << "Pitch: " << pitch << "\n";
        ss << "Speed: " << (int)airplane.speed;
        infoText.setString(ss.str());

        // Ekranı temizle
        window.clear(sf::Color::White);

        // Uçağı çiz
        sf::CircleShape airplaneShape(10);
        airplaneShape.setFillColor(sf::Color::Red);
        airplaneShape.setPosition(airplane.position);
        window.draw(airplaneShape);

        // Dinleyiciyi çiz
        sf::CircleShape listenerShape(5);
        listenerShape.setFillColor(sf::Color::Blue);
        listenerShape.setPosition(listenerPosition);
        window.draw(listenerShape);

        // Slider bar'ı ve başlığını çiz
        window.draw(sliderBar);
        window.draw(sliderThumb);

        // Bilgi metnini çiz
        window.draw(infoText);

        // Ekranı göster
        window.display();
    }

    return EXIT_SUCCESS;
}
