# Radiance Cascades GI for O3DE

Real‑time, noiseless global illumination and specular reflections using hierarchical Radiance Cascades.  
A modular Gem for the **Open 3D Engine** (O3DE) that delivers stable, ghosting‑free indirect lighting and 360° reflections without screen‑space limitations or per‑frame denoising.

---

## ✨ Features

- **Noiseless GI & Reflections** – Hierarchical radiance field decomposition into cascades eliminates Monte Carlo noise.
- **Hybrid Injection Architecture** – Fast software rasterization for broad compatibility, with an optional hardware ray‑tracing path for maximum fidelity.
- **Sparse Clipmap Probe Grid** – Only probes that actually see geometry are active; memory footprint is bound by a fixed pool.
- **Adaptive Temporal Accumulation** – Per‑probe blend weight adapts to lighting changes, avoiding ghosting while keeping the image smooth.
- **Full Off‑Screen Reflections** – No screen‑space fallbacks; reflections are captured from all directions.
- **Editor Component** – Configure probe spacing, volume size, injection mode, and temporal blend directly in the O3DE Editor property grid.

---

## 📦 Status

- ✅ C++ skeleton compiles and registers the feature processor
- ✅ Editor component with property grid UI (injection mode, probe spacing, volume size, temporal weight)
- ✅ Console CVars (`r_radianceCascade_mode`, `r_radianceCascade_temporalWeight`) are live
- ✅ Debug compute shader (`CascadeInject_Software.azsl`) compiles successfully


---

## 🛠️ Building the Gem

### Prerequisites

- **O3DE 25.10** (or compatible version) installed and registered.
- A project created with the O3DE Project Manager.
- Visual Studio 2022 (Windows) with C++ development workload.

### Build steps

1. Place the `RadianceCascade` folder inside your project’s `Gems/` directory (or keep it external and register it).
2. Enable the Gem for your project
3. build your project