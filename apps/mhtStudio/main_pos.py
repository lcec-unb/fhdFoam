import tkinter as tk
from tkinter import *
from tkinter import simpledialog, messagebox, Tk, Frame, Label, LEFT, RIGHT, Button, Entry, font

import customtkinter as cttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import numpy as np
import os
import json
import matplotlib.pyplot as plt
from matplotlib.patches import Circle

from pathlib import Path
import threading
import subprocess
import sys
import time

HERE = Path(__file__).resolve().parent
os.chdir(HERE)


class Main_wind_pos:

    # ----------------------------
    # Progress window helpers
    # ----------------------------
    def show_progress_window(self, text="Running postProcess..."):
        self.progress_win = cttk.CTkToplevel(self.root_1)
        self.progress_win.title("mhtStudio — Processing")
        self.progress_win.geometry("460x180")
        self.progress_win.attributes("-topmost", True)

        frame = cttk.CTkFrame(self.progress_win)
        frame.pack(fill="both", expand=True, padx=15, pady=15)

        self.progress_label = cttk.CTkLabel(frame, text=text)
        self.progress_label.pack(pady=(10, 6))

        self.progress_bar = cttk.CTkProgressBar(frame, mode="indeterminate")
        self.progress_bar.pack(fill="x", padx=10, pady=(6, 10))
        self.progress_bar.start()

        # garante que desenhou antes de tentar grab
        self.progress_win.update_idletasks()
        self.progress_win.after(50, lambda: self._safe_grab())

    def _safe_grab(self):
        # grab_set pode falhar se a janela ainda não estiver "viewable"
        try:
            self.progress_win.grab_set()
        except Exception:
            pass

    def update_progress_text(self, text):
        if hasattr(self, "progress_label") and self.progress_label:
            self.progress_label.configure(text=text)

    def close_progress_window(self):
        try:
            if hasattr(self, "progress_bar") and self.progress_bar:
                self.progress_bar.stop()
        except Exception:
            pass
        try:
            if hasattr(self, "progress_win") and self.progress_win:
                self.progress_win.destroy()
        except Exception:
            pass

    # ----------------------------
    # Main GUI
    # ----------------------------
    def __init__(self, root_1):
        self.root_1 = root_1
        self.root_1.title("mhtFoam post-processing")
        cttk.set_appearance_mode("Dark")

        largura_tela_1 = self.root_1.winfo_screenwidth()
        altura_tela_1 = self.root_1.winfo_screenheight()
        self.largura_tela = largura_tela_1 // 4
        self.altura_tela = altura_tela_1 // 3
        self.root_1.geometry(f'1000x500+{self.largura_tela}+{self.altura_tela}')

        self.extra_data_entries = {}
        self.data_t = {"extra": []}
        self.extra_windows = []

        self.root_1.protocol("WM_DELETE_WINDOW", self.on_close)

    def on_close(self):
        try:
            self.root_1.destroy()
        except Exception:
            pass
        raise SystemExit(0)

    def interface(self):
        self.define_titulo()
        self.view_screen()
        self.user_data()

    def define_titulo(self):
        primeiroContainer = cttk.CTkFrame(self.root_1, corner_radius=15, fg_color="#2B2B2B")
        primeiroContainer.pack(pady=10, padx=25, fill="x")

        texto1 = "Welcome to the mhtFoam post-processing interface! Here you can visualize the results of the mhtFoam solver."
        titulo = cttk.CTkLabel(
            primeiroContainer,
            text=texto1,
            font=("Ubuntu", 15, "bold"),
            text_color="white",
            wraplength=650,
            justify="center"
        )
        titulo.pack(pady=8, padx=15)

    def view_screen(self):
        view_screen = cttk.CTkFrame(self.root_1, corner_radius=40)
        view_screen.pack(side=LEFT, pady=10, padx=25)

        with open('inputDict_blockMeshDict.json', 'r') as json_file:
            domain_info = json.load(json_file)
        with open('inputDict_ID.json', 'r') as json_file:
            domain_info3 = json.load(json_file)
        with open('inputDict_magflu.json', 'r') as json_file:
            domain_info4 = json.load(json_file)

        xmax = float(domain_info["xmax"])
        ymax = float(domain_info["ymax"])

        fig, ax = plt.subplots(figsize=(5, 5), dpi=100)
        ax.set_xlim(0, xmax)
        ax.set_ylim(0, ymax)
        ax.set_xlabel("X")
        ax.set_ylabel("Y")
        ax.set_aspect('equal')
        ax.set_title("Domínio da Simulação")
        ax.grid(True, which='both', linestyle='--', linewidth=0.5, color='gray', alpha=0.7)

        # Tumores
        tumor_count = 1
        if "tumors" in domain_info3 and domain_info3["tumors"]:
            for tumor in domain_info3["tumors"]:
                try:
                    radius = float(tumor[f"radius_{tumor_count}"])
                    eccen = float(tumor[f"eccen_{tumor_count}"])
                    posx = float(tumor[f"posx_{tumor_count}"])
                    posy = float(tumor[f"posy_{tumor_count}"])
                    inclination = float(tumor[f"inclination_{tumor_count}"])
                except KeyError:
                    tumor_count += 1
                    continue

                if eccen == 0:
                    ax.add_patch(Circle((posx, posy), radius, color='blue'))
                else:
                    inc = np.radians(inclination)
                    t = np.linspace(0, 2 * np.pi, 100)
                    ellipse_x = radius / (np.sqrt(1 - eccen**2)) * np.cos(t)
                    ellipse_y = radius * np.sin(t)
                    x_rot = posx + ellipse_x * np.cos(inc) - ellipse_y * np.sin(inc)
                    y_rot = posy + ellipse_x * np.sin(inc) + ellipse_y * np.cos(inc)
                    ax.fill(x_rot, y_rot, color='blue')

                tumor_count += 1

        # Fluidos magnéticos
        fluid_count = 1
        if "magnetic_fluid" in domain_info4 and domain_info4["magnetic_fluid"]:
            for fluid in domain_info4["magnetic_fluid"]:
                try:
                    posxm = float(fluid[f"posx_{fluid_count}"])
                    posym = float(fluid[f"posy_{fluid_count}"])
                    volume = float(fluid[f"volume_{fluid_count}"])
                    volume_mag = volume * 1e-6
                    radius = ((3 * volume_mag) / (4 * np.pi)) ** (1 / 3)
                    ax.add_patch(Circle((posxm, posym), radius, color='black'))
                except Exception:
                    pass
                fluid_count += 1

        canvas = FigureCanvasTkAgg(fig, master=view_screen)
        canvas.draw()
        canvas.get_tk_widget().pack()

    def user_data(self):
        coleta_main_data = cttk.CTkFrame(self.root_1, corner_radius=15)
        coleta_main_data.pack(side=RIGHT, pady=10, padx=25)

        coleta_data = cttk.CTkFrame(coleta_main_data, corner_radius=15)
        coleta_data.pack(pady=10, padx=25)

        extra_count_numlabel = cttk.CTkLabel(coleta_data, text="Number of extraction points: ")
        extra_count_numlabel.pack(side="left")

        self.extra_count_num = cttk.CTkEntry(coleta_data, width=300, placeholder_text="2")
        self.extra_count_num.pack(side="right")

        tumorbutton = cttk.CTkButton(
            coleta_main_data,
            text="Ok",
            width=150,
            height=40,
            command=self.submit_extra
        )
        tumorbutton.pack(side=BOTTOM, pady=5, padx=5)

    def submit_extra(self):
        points_count = int(self.extra_count_num.get())
        self.open_extra_data_screens(points_count)

    def open_extra_data_screens(self, points_count):
        self.extra_data_entries = {}
        self.data_t = {"extra": []}
        self.extra_windows = []

        for i in range(points_count):
            lx = self.largura_tela + (i - 1) * 80
            ly = self.altura_tela + (i - 1) * 100
            self.collect_extra_data(i, points_count, lx, ly)

    def collect_extra_data(self, index, points_count, lx, ly):
        self.extra_data_entries[index] = {}

        win = cttk.CTkToplevel(self.root_1)
        win.title(f"Point {index + 1} of extraction")
        win.geometry(f'300x150+{lx}+{ly}')
        self.extra_windows.append(win)

        c1 = cttk.CTkFrame(win)
        c1.pack(pady=10, padx=20)
        cttk.CTkLabel(c1, text="x position: ").pack(side="left")
        posx = cttk.CTkEntry(c1, width=300, placeholder_text="0.045")
        posx.pack(side="left")

        c2 = cttk.CTkFrame(win)
        c2.pack(pady=10, padx=20)
        cttk.CTkLabel(c2, text="y position: ").pack(side="left")
        posy = cttk.CTkEntry(c2, width=300, placeholder_text="0.045")
        posy.pack(side="right")

        self.extra_data_entries[index]["posx"] = posx
        self.extra_data_entries[index]["posy"] = posy

        cttk.CTkButton(
            win,
            text="Ok",
            width=150,
            height=40,
            command=lambda w=win, idx=index, total=points_count: self.gera_json_extra(w, idx, total)
        ).pack(side=BOTTOM, padx=5)

    def gera_json_extra(self, window, index, points_count):
        indexx = index + 1
        inputDict_extra = {
            f"posx_{indexx}": self.extra_data_entries[index]["posx"].get(),
            f"posy_{indexx}": self.extra_data_entries[index]["posy"].get(),
        }
        self.data_t["extra"].append(inputDict_extra)

        out_json = "inputDict_extraction.json"
        if len(self.data_t["extra"]) == points_count:
            with open(out_json, "w") as f:
                json.dump(self.data_t, f, indent=4)

            for w in list(self.extra_windows):
                try:
                    w.destroy()
                except Exception:
                    pass
            self.extra_windows = []

            # janela de progresso + thread
            self.show_progress_window("Configuring probes & running postProcess...")
            self.start_processing()
        else:
            try:
                window.destroy()
            except Exception:
                pass

    # ----------------------------
    # Worker thread
    # ----------------------------
    def start_processing(self):
        worker = threading.Thread(target=self._processing_worker, daemon=True)
        worker.start()

    def _processing_worker(self):
        try:
            self.root_1.after(0, lambda: self.update_progress_text("Updating controlDict probeLocations..."))
            self.change_file()

            self.root_1.after(0, lambda: self.update_progress_text("Running postProcess for T and W..."))
            self.postprocess_with_live_progress()

            self.root_1.after(0, lambda: self.update_progress_text("Generating plots..."))
        except Exception as e:
            self.root_1.after(0, lambda: messagebox.showerror("Error", str(e)))
            self.root_1.after(0, self.close_progress_window)
            return

        self.root_1.after(0, self.close_progress_window)
        self.root_1.after(200, self._shutdown_and_launch_plots)

    def _shutdown_and_launch_plots(self):
        try:
            self.root_1.destroy()
        except Exception:
            pass

        try:
            subprocess.run([sys.executable, str(HERE / "plots.py")], check=True)
        finally:
            raise SystemExit(0)

    # ----------------------------
    # Update probeLocations
    # ----------------------------
    def change_file(self):
        control_dict_path = os.environ.get("control_arc")
        if not control_dict_path or not os.path.isfile(control_dict_path):
            raise RuntimeError(f"controlDict not found: {control_dict_path}")

        with open(control_dict_path, 'r') as file:
            control_dict = file.readlines()

        with open('inputDict_extraction.json', 'r') as json_file:
            dados_pontos = json.load(json_file)

        extra_data = dados_pontos.get("extra", [])
        if not extra_data:
            raise RuntimeError("No extraction points found in inputDict_extraction.json")

        new_block = []
        for i, ponto in enumerate(extra_data, start=1):
            posx = ponto.get(f"posx_{i}")
            posy = ponto.get(f"posy_{i}")
            new_block.append(f"\t\t\t({posx} {posy} 0)\n")

        idx_probe = None
        for i, line in enumerate(control_dict):
            if line.strip().startswith("probeLocations"):
                idx_probe = i
                break
        if idx_probe is None:
            raise RuntimeError("Could not find 'probeLocations' in controlDict.")

        idx_open = None
        for j in range(idx_probe, len(control_dict)):
            if control_dict[j].strip() == "(":
                idx_open = j
                break
        if idx_open is None:
            raise RuntimeError("Could not find '(' after probeLocations.")

        idx_close = None
        for k in range(idx_open, len(control_dict)):
            if control_dict[k].strip() == ");":
                idx_close = k
                break
        if idx_close is None:
            raise RuntimeError("Could not find closing ');' for probeLocations.")

        updated = control_dict[:idx_open + 1] + new_block + control_dict[idx_close:]

        with open(control_dict_path, 'w') as file:
            file.writelines(updated)

    # ----------------------------
    # Live progress helper: read last time from probes T
    # ----------------------------
    def _read_last_probe_time(self, t_file: Path):
        """
        Returns last time value (float) found in probe file, or None.
        """
        try:
            if not t_file.is_file():
                return None
            with t_file.open("r") as f:
                lines = f.readlines()
            # find last numeric line
            for line in reversed(lines):
                s = line.strip()
                if not s or s.startswith("#") or s.lower().startswith("time"):
                    continue
                parts = s.split()
                try:
                    return float(parts[0])
                except Exception:
                    continue
            return None
        except Exception:
            return None

    def _get_endtime_seconds(self):
        """
        Reads endtime from inputDict_controlDict.json in mhtStudio folder.
        Returns int or None.
        """
        try:
            p = HERE / "inputDict_controlDict.json"
            if not p.is_file():
                return None
            with p.open("r") as f:
                d = json.load(f)
            # endtime may be str
            return int(float(d.get("endtime")))
        except Exception:
            return None

    # ----------------------------
    # Run postProcess with live progress updates
    # ----------------------------
    def postprocess_with_live_progress(self):
        caso_dir = os.environ.get("mht_tutorials")
        if not caso_dir or not os.path.isdir(caso_dir):
            raise RuntimeError(f"Case directory not found: {caso_dir}")

        endtime = self._get_endtime_seconds()

        os.chdir(caso_dir)

        # Run postProcess (as before) but using Popen so we can update progress
        proc = subprocess.Popen(
            ['postProcess', '-fields', '(W T)'],
            text=True
        )

        # We'll monitor postProcessing/probes/0/T as it grows
        probes_t = Path(caso_dir) / "postProcessing" / "probes" / "0" / "T"

        last_reported = None
        start_time = time.time()

        while True:
            ret = proc.poll()

            # read last time from T file if available
            t_last = self._read_last_probe_time(probes_t)
            if t_last is not None:
                # avoid spamming UI; update only if changed
                if last_reported is None or (t_last != last_reported):
                    last_reported = t_last
                    if endtime:
                        pct = max(0, min(100, int(round(100 * t_last / endtime))))
                        msg = f"Running postProcess... t = {t_last:.3f} s  ({pct}%)"
                    else:
                        msg = f"Running postProcess... t = {t_last:.3f} s"
                    self.root_1.after(0, lambda m=msg: self.update_progress_text(m))
            else:
                # early stage: file not created yet
                elapsed = int(time.time() - start_time)
                msg = f"Running postProcess... (initializing)  [{elapsed}s]"
                self.root_1.after(0, lambda m=msg: self.update_progress_text(m))

            if ret is not None:
                break

            time.sleep(0.5)

        # Collect outputs to catch errors cleanly
        ret = proc.wait()
        if ret != 0:
            raise RuntimeError(f"postProcess failed (code {ret}). Check run_pos.log for details.")


        if proc.returncode != 0:
            # include some stderr (trim) in the exception
            err_tail = (err or "")[-2000:]
            raise RuntimeError(f"postProcess failed (code {proc.returncode}).\n\n{err_tail}")


root_1 = cttk.CTk()
app = Main_wind_pos(root_1)
app.interface()
root_1.mainloop()

