# 🎧 Doppler Effect Simulation with SFML

Bu proje, **SFML (Simple and Fast Multimedia Library)** kullanılarak oluşturulmuş bir **Doppler etkisi simülasyonudur**. Ekranda yatay eksende hareket eden bir uçak nesnesi vardır ve bu nesneye bağlı ses kaynağının frekansı (pitch) ve sesi (volume), dinleyiciye göre dinamik olarak değişir.

---

## 🛩️ Projenin Amacı

Gerçek hayatta bir ambulans ya da uçak yanımızdan geçerken sesin tizliği yaklaşırken artar, uzaklaşırken azalır. Bu fiziksel olaya **Doppler etkisi** denir. Bu projede:

- Uçak ekranın bir ucundan diğerine geçerken,
- Dinleyici sabit bir noktada dururken,
- Uçağın sesi, **uzaklık** ve **göreli hız** bilgisine göre **otomatik olarak değişmektedir**.

---

## 🖼️ Ekran Görünümü

```
📦 Uçak: Kırmızı daire
🎧 Dinleyici: Mavi daire
🎚️ Hız kontrolü: Kaydırıcı (slider)
📝 Bilgi Paneli: Anlık mesafe, ses seviyesi, pitch ve hız bilgisi
```

---

## 📐 Doppler Efekti Mantığı

### 🔬 Doppler Etkisi Nedir?

Bir ses kaynağı hareket ettiğinde, kaynağın ürettiği dalgalar sıkışır veya seyrelir:

- **Yaklaşırken:** Ses dalgaları sıkışır → frekans artar → **tizleşir**
- **Uzaklaşırken:** Ses dalgaları seyrelir → frekans azalır → **pesleşir**

---

### 📊 Kullanılan Formül

```cpp
pitch = speedOfSound / (speedOfSound ± relativeVelocity)
```

- `speedOfSound`: Sabit, genellikle 343 m/s (hava içinde).
- `relativeVelocity`: Uçağın dinleyiciye olan göreli hızı (x ekseni).
- Eğer uçak yaklaşıyorsa `-` (frekans artar),
- Uzaklaşıyorsa `+` (frekans düşer) kullanılır.

#### 🎛️ Ekstra Ayarlama

Gerçek Doppler etkisi bazen fazla sert olabilir. Bunu yumuşatmak için bir **"dopplerStrength"** çarpanı ile etki azaltılır:

```cpp
pitch = 1.0f + (rawPitch - 1.0f) * dopplerStrength;
```

- `dopplerStrength = 0.3f` → Etkinin %30’u uygulanır.
- Pitch değeri `0.5` ile `2.0` arasında sınırlandırılır (duyulabilirlik için).

---

## ⚙️ Kullanım

### 1️⃣ Derlemek İçin Gerekenler

- [SFML 2.x](https://www.sfml-dev.org/)
- C++17 veya üstü destekleyen bir derleyici (GCC / Clang / MSVC)

### 2️⃣ Kaynak Dosyalar

- `aygaz_music.ogg` → Uçağın çıkardığı ses
- `tuffy.ttf` → Bilgi metni için yazı tipi

### 3️⃣ Derleme Komutu (örnek)

```bash
g++ main.cpp -o doppler_sim -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
```

---

## 🎮 Kontroller

| Eylem               | Açıklama                                           |
| ------------------- | -------------------------------------------------- |
| Kaydırıcıyı sürükle | Uçağın hızını artır/azalt                          |
| Ekranı izle         | Sesin tizliğinin ve şiddetinin değişimini gözlemle |

---

## 📌 Projenin Özellikleri

- ✅ Gerçek zamanlı fiziksel hesaplamalar
- ✅ Pitch (frekans) ve volume (ses seviyesi) simülasyonu
- ✅ Basit ama etkili grafik arayüz
- ✅ Kaydırıcı ile hız kontrolü

---

## 👨‍💻 Geliştirici

**Halil Öksüz**
