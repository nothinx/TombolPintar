"""Compile + jalankan simulasi.cpp (kode TombolPintar asli), lalu gambar grafik ke ../gambar/."""
import glob, os, subprocess, sys, tempfile

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

plt.rcParams.update({
    "figure.figsize": (8, 3.6), "figure.dpi": 100, "savefig.bbox": "tight", "savefig.pad_inches": 0.15,
    "figure.facecolor": "white", "axes.facecolor": "white", "savefig.facecolor": "white",
    "font.size": 10, "axes.titlesize": 11, "axes.titleweight": "bold", "axes.titlelocation": "left",
    "axes.spines.top": False, "axes.spines.right": False, "axes.edgecolor": "#9ca3af",
    "axes.grid": True, "grid.color": "#e5e7eb", "grid.linewidth": 0.8,
    "legend.frameon": False, "svg.fonttype": "path", "svg.hashsalt": "nothinx",
    "lines.linewidth": 1.8,
})
WARNA = {"utama": "#2563eb", "pembanding": "#dc2626", "ketiga": "#16a34a", "keempat": "#9333ea",
         "kelima": "#ea580c", "mentah": "#9ca3af", "target": "#111827"}

SINI = os.path.dirname(os.path.abspath(__file__))
KELUAR = os.path.join(SINI, "..", "gambar")
EVENT = ["ditekan()", "dilepas()", "diklik()", "diklikGanda()", "ditekanLama()", "berulang()"]


def jalankan():
    exe = os.path.join(tempfile.mkdtemp(), "sim")
    src = sorted(glob.glob(os.path.join(SINI, "..", "..", "src", "*.cpp")))
    subprocess.run(["g++", "-std=c++11", "-O2", "-I" + os.path.join(SINI, "..", "test"),
                    "-I" + os.path.join(SINI, "..", "..", "src"), os.path.join(SINI, "simulasi.cpp"), *src,
                    "-o", exe], check=True)
    keluaran = subprocess.run([exe], check=True, capture_output=True, text=True).stdout
    data, nama = {}, None
    for baris in keluaran.splitlines():
        if baris.startswith("# "):
            nama = baris[2:]
            data[nama] = []
        elif baris[0].isdigit():
            t, *sisa = baris.split(",")
            data[nama].append([float(t)] + [int(x) for x in sisa])
    return data


def waktu_event(baris, bit):
    return [b[0] for b in baris if b[3] >> bit & 1]


def gambar_pin(ax, baris, xlim=None):
    t = [b[0] for b in baris]
    ax.step(t, [b[1] for b in baris], where="post", color=WARNA["mentah"], lw=1.0, label="pin mentah")
    ax.step(t, [0 if b[2] else 1 for b in baris], where="post", color=WARNA["utama"], lw=1.4, alpha=0.55,
            label="status di library")
    ax.set_yticks([0, 1], ["LOW", "HIGH"])
    ax.set_ylim(-0.25, 1.25)
    if xlim:
        ax.set_xlim(*xlim)


def gambar_event(ax, baris, durasi):
    for i, nama in enumerate(EVENT):
        t = waktu_event(baris, i)
        ax.plot(t, [i] * len(t), "o", color=WARNA["utama"], ms=6)
    ax.set_yticks(range(len(EVENT)), EVENT)
    ax.set_ylim(len(EVENT) - 0.4, -0.6)
    ax.set_xlim(0, durasi)
    ax.set_xlabel("waktu (ms)")


def durasi(baris):
    return baris[-1][0]


def judul(fig, teks):
    fig.suptitle(teks, x=0.02, ha="left", fontsize=11, fontweight="bold")


def simpan(fig, nama):
    fig.savefig(os.path.join(KELUAR, nama), format="svg", metadata={"Date": None})
    plt.close(fig)


def selang(ax, x0, x1, y, teks):
    ax.annotate("", (x1, y), (x0, y), arrowprops=dict(arrowstyle="<->", color=WARNA["target"], lw=1.0))
    ax.text((x0 + x1) / 2, y - 0.25, teks, ha="center", va="bottom", fontsize=9)


def main():
    os.makedirs(KELUAR, exist_ok=True)
    data = jalankan()

    # 1. Satu klik: getaran kontak, ditekan() instan, diklik() setelah jeda klik.
    b = data["klik"]
    sentuh = next(x[0] for x in b if x[1] == 0)
    lepas = waktu_event(b, 1)[0]
    tekan, klik = waktu_event(b, 0)[0], waktu_event(b, 2)[0]
    fig = plt.figure(figsize=(8, 4.6))
    gs = fig.add_gridspec(2, 3, height_ratios=[1, 1.5], hspace=0.45, wspace=0.35)
    ax = fig.add_subplot(gs[0, :2])
    gambar_pin(ax, b, (0, durasi(b)))
    judul(fig, f"Satu klik: ditekan() {tekan - sentuh:.0f} ms setelah sentuhan pertama, "
               f"diklik() {klik - lepas:.0f} ms setelah dilepas")
    ax.text(300, 0.5, "pin mentah (abu-abu)\nstatus di library (biru)\nLOW = ditekan", va="center",
            fontsize=8.5, color="#4b5563")
    az = fig.add_subplot(gs[0, 2])
    gambar_pin(az, b, (sentuh - 4, sentuh + 26))
    az.axvspan(sentuh, sentuh + 20, color=WARNA["utama"], alpha=0.08, lw=0)
    az.text(sentuh + 12, 0.7, "debounce\n20 ms", ha="center", va="center", fontsize=8.5)
    az.annotate("ditekan()", (sentuh, 0), (sentuh + 5, 0.15), fontsize=8.5, color=WARNA["utama"],
                arrowprops=dict(arrowstyle="->", color=WARNA["utama"], lw=1.0))
    az.set_title("diperbesar: getaran saat ditekan", fontsize=9.5, fontweight="normal")
    az.set_xlabel("waktu (ms)")
    ae = fig.add_subplot(gs[1, :])
    gambar_event(ae, b, durasi(b))
    selang(ae, lepas, klik, 2, f"jeda klik {klik - lepas:.0f} ms")
    simpan(fig, "tombol_klik.svg")

    # 2. Klik ganda.
    b = data["klik_ganda"]
    lepas2 = waktu_event(b, 1)[-1]
    ganda = waktu_event(b, 3)[0]
    n_klik = len(waktu_event(b, 2))
    fig, (ax, ae) = plt.subplots(2, 1, figsize=(8, 4.0), sharex=True, height_ratios=[1, 1.6])
    gambar_pin(ax, b)
    klik_tunggal = "diklik() tidak muncul" if n_klik == 0 else f"diklik() muncul {n_klik} kali"
    judul(fig, f"Klik ganda: diklikGanda() {ganda - lepas2:.0f} ms setelah lepas kedua, {klik_tunggal}")
    gambar_event(ae, b, durasi(b))
    selang(ae, lepas2, ganda, 3, f"{ganda - lepas2:.0f} ms")
    simpan(fig, "tombol_klik_ganda.svg")

    # 3. Tahan 2 detik.
    b = data["tahan"]
    sentuh = next(x[0] for x in b if x[1] == 0)
    lama = waktu_event(b, 4)[0]
    ulang = waktu_event(b, 5)
    fig, (ax, ae) = plt.subplots(2, 1, figsize=(8, 4.0), sharex=True, height_ratios=[1, 1.6])
    gambar_pin(ax, b)
    judul(fig, f"Tahan 2 detik: ditekanLama() setelah {lama - sentuh:.0f} ms, berulang() tiap "
               f"{ulang[1] - ulang[0]:.0f} ms, {'lepas bukan klik' if not waktu_event(b, 2) else 'lepas terhitung klik'}")
    gambar_event(ae, b, durasi(b))
    simpan(fig, "tombol_tahan.svg")


if __name__ == "__main__":
    sys.exit(main())
