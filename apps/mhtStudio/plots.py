class main_pos:
    def __init__(self):
        print("Criando gráficos...")

    def _find_data_start(self, lines):
        for i, line in enumerate(lines):
            s = line.strip()
            if s.startswith("Time") or s.startswith("# Time") or ("Time" in s and "Time" in s.split()):
                return i + 1
        return None

    def _read_probe_file(self, path):
        import numpy as np
        with open(path, 'r') as f:
            lines = f.readlines()

        start = self._find_data_start(lines)
        if start is None:
            numeric_start = None
            for i, line in enumerate(lines):
                s = line.strip()
                if not s or s.startswith("#"):
                    continue
                try:
                    float(s.split()[0])
                    numeric_start = i
                    break
                except Exception:
                    continue
            if numeric_start is None:
                raise ValueError(f"Could not find numeric data in probe file: {path}")
            start = numeric_start

        data = np.loadtxt(lines[start:], dtype=float)
        t = data[:, 0]
        vals = data[:, 1:]
        return t, vals

    def visual_pos(self):
        import os
        import numpy as np
        import matplotlib
        matplotlib.use('QtAgg')
        import matplotlib.pyplot as plt
        import json

        script_dir = os.path.dirname(os.path.realpath(__file__))
        os.chdir(script_dir)

        mht_tutorials = os.environ.get("mht_tutorials")
        if not mht_tutorials:
            raise RuntimeError("mht_tutorials not set")

        probes_dir = os.path.join(mht_tutorials, "postProcessing", "probes", "0")
        file_path_T = os.path.join(probes_dir, "T")
        file_path_W = os.path.join(probes_dir, "W")

        time, temperatures = self._read_probe_file(file_path_T)
        time2, perfusion = self._read_probe_file(file_path_W)

        plt.figure(figsize=(10, 6))
        for i in range(temperatures.shape[1]):
            plt.plot(time, temperatures[:, i], label=f'Point {i+1}')
        plt.title('Temperature at probe points')
        plt.xlabel('Time (s)')
        plt.ylabel('Temperature (K)')
        plt.legend()
        plt.grid(True)
        plt.show()
        plt.close()

        plt.figure(figsize=(10, 6))
        for i in range(perfusion.shape[1]):
            plt.plot(time2, perfusion[:, i], label=f'Point {i+1}')
        plt.title('Blood perfusion at probe points')
        plt.xlabel('Time (s)')
        plt.ylabel('Perfusion (1/s)')
        plt.legend()
        plt.grid(True)
        plt.show()
        plt.close()

        with open('inputDict_blockMeshDict.json', 'r') as json_file:
            domain_info = json.load(json_file)
        with open('inputDict_controlDict.json', 'r') as json_file:
            domain_info2 = json.load(json_file)

        xmax = float(domain_info["xmax"])
        ymax = float(domain_info["ymax"])
        xnode = int(domain_info["xnode"])
        ynode = int(domain_info["ynode"])

        endtime = int(domain_info2["endtime"])
        file_path3 = os.path.join(mht_tutorials, f'{endtime}', 'T')

        with open(file_path3, 'r') as file:
            lines = file.readlines()

        start_line = None
        for i, line in enumerate(lines):
            if line.strip().startswith("internalField"):
                start_line = i + 1
                break

        num_temperatures = int(lines[start_line].strip())
        temperature_data = [float(lines[start_line + 2 + i].strip()) for i in range(num_temperatures)]
        temperatures2d = np.array(temperature_data).reshape((ynode, xnode))

        x = np.linspace(0, xmax, xnode)
        y = np.linspace(0, ymax, ynode)
        X, Y = np.meshgrid(x, y)

        fig_2d, ax_2d = plt.subplots(figsize=(10, 8))
        contour = ax_2d.contourf(X, Y, temperatures2d, cmap='plasma')

        fig_2d.colorbar(contour, ax=ax_2d, label="T(K)")
        ax_2d.set_title(f'Temperature distribution at the instant {endtime} s')
        ax_2d.set_xlabel('X size (m)')
        ax_2d.set_ylabel('Y size (m)')
        plt.show()
        plt.close()


if __name__ == "__main__":
    app = main_pos()
    app.visual_pos()
