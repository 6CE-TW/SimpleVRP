window.addEventListener('DOMContentLoaded', () => {
  const map = L.map('map').setView([25.0478, 121.5171], 13);

  const dayTile = L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    maxZoom: 19,
    attribution: '© OpenStreetMap'
  });

  const nightTile = L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png', {
    maxZoom: 19,
    subdomains: 'abcd',
    attribution: '&copy; <a href="https://carto.com/">CARTO</a>'
  });

  let isNight = false;
  dayTile.addTo(map);

  const toggleButton = document.getElementById('toggle-theme');
  toggleButton.addEventListener('click', () => {
    if (isNight) {
      map.removeLayer(nightTile);
      dayTile.addTo(map);
      toggleButton.innerText = '🌞 Switch to Night Mode';
      document.body.classList.remove('night-mode');
    } else {
      map.removeLayer(dayTile);
      nightTile.addTo(map);
      toggleButton.innerText = '🌙 Switch to Day Mode';
      document.body.classList.add('night-mode');
    }
    isNight = !isNight;
  });

  const mapPane = document.querySelector('.leaflet-map-pane');
  const overlayDiv = document.createElement('div');
  overlayDiv.className = 'tile-overlay';
  mapPane.appendChild(overlayDiv);

  const vehicleLayers = {};
  const layerVisibility = {};
  const vehicleInfoMap = {};
  let currentTab = "all";

  const updateVehicleVisibility = () => {
    Object.keys(vehicleLayers).forEach(id => {
      map.removeLayer(vehicleLayers[id]);
    });

    if (currentTab === "all") {
      Object.entries(vehicleLayers).forEach(([id, layer]) => {
        if (layerVisibility[id]) {
          layer.addTo(map);
        }
      });
    } else {
      const layer = vehicleLayers[currentTab];
      if (layer) {
        layer.addTo(map);
        const bounds = layer.getBounds?.();
        if (bounds) map.fitBounds(bounds);
      }
    }
  };

  const renderSidebarButtons = (vehicleIds) => {
    const container = document.getElementById("sidebar-buttons");
    container.innerHTML = "";

    // All button
    const allButton = document.createElement("button");
    allButton.textContent = "All";
    allButton.className = "vehicle-button" + (currentTab === "all" ? " active" : "");
    allButton.onclick = () => {
      currentTab = "all";
      updateVehicleVisibility();
      renderSidebarButtons(vehicleIds);
    };
    container.appendChild(allButton);

    // Checkbox group (only when All is selected)
    if (currentTab === "all") {
      const group = document.createElement("div");
      group.className = "vehicle-checkbox-group";

      vehicleIds.forEach(id => {
        const checkbox = document.createElement("input");
        checkbox.type = "checkbox";
        checkbox.id = `checkbox_${id}`;
        checkbox.checked = layerVisibility[id];

        checkbox.onchange = () => {
          layerVisibility[id] = checkbox.checked;
          updateVehicleVisibility();
        };

        const label = document.createElement("label");
        label.htmlFor = checkbox.id;
        label.textContent = `Vehicle ${id}`;

        const row = document.createElement("div");
        row.appendChild(checkbox);
        row.appendChild(label);
        group.appendChild(row);
      });

      container.appendChild(group);
    }

    // Vehicle buttons
    vehicleIds.forEach(id => {
      const btn = document.createElement("button");
      btn.className = "vehicle-button" + (currentTab === id ? " active" : "");
      btn.textContent = `Vehicle ${id}`;
      btn.onclick = () => {
        currentTab = id;
        updateVehicleVisibility();
        renderSidebarButtons(vehicleIds);
      };
      container.appendChild(btn);
    });

    // Display Vehicle Information
    if (currentTab !== "all" && vehicleLayers[currentTab]) {
      const info = document.createElement("div");
      info.className = "vehicle-info";
      info.style.marginTop = "10px";
      info.style.padding = "6px";
      info.style.fontSize = "13px";
      info.style.borderTop = "1px solid #ccc";

      const infoData = vehicleInfoMap[currentTab] || {};
      info.innerHTML = `
        <strong>Vehicle ${currentTab} Information</strong><br/>
        Number of Nodes: ${infoData.node_count ?? "-"}<br/>
        Route Length: ${infoData.distance / 1000 ?? "-"} KM<br/>
        Estimated Time: ${infoData.duration ?? "-"} min
      `;

      container.appendChild(info);
    }
  };

  fetch("http://localhost:8080/get-navigation")
    .then(res => res.json())
    .then(geojson => {
      const vehicleData = {};

      geojson.features.forEach(feature => {
        const id = feature.properties?.vehicle_id || "unknown";
        if (!vehicleData[id]) {
          vehicleData[id] = { lines: [], points: [] };
        }

        if (feature.geometry.type === "LineString") {
          vehicleData[id].lines.push(feature);
        } else if (feature.geometry.type === "Point") {
          vehicleData[id].points.push(feature);
        }
      });

      Object.entries(vehicleData).forEach(([id, data]) => {
        const lineLayer = L.geoJSON(data.lines, {
          style: feature => ({
            color: feature.properties?.stroke || "#3388ff",
            weight: feature.properties?.["stroke-width"] || 3,
            opacity: feature.properties?.["stroke-opacity"] || 0.8
          })
        });

        const pointLayer = L.geoJSON(data.points, {
          pointToLayer: (feature, latlng) => {
            const color = feature.properties?.["marker-color"] || "#3388ff";
            const marker = L.circleMarker(latlng, {
              radius: 8,
              fillColor: color,
              stroke: false,
              color: "#000",
              weight: 1,
              opacity: 1,
              fillOpacity: 0.8
            });
            marker.bindPopup(`Name: ${feature.properties?.name}<br>Seq: ${feature.properties?.serial}`);
            marker.bindTooltip(`No.${feature.properties?.serial}`, { direction: 'top' });
            return marker;
          }
        });

        const group = L.layerGroup([lineLayer, pointLayer]);
        vehicleLayers[id] = group;
        layerVisibility[id] = true;

        const infoLine = data.lines[0]?.information || {};
        vehicleInfoMap[id] = {
          distance: infoLine.distance ?? 0,
          duration: infoLine.duration ?? 0,
          node_count: infoLine.node_count ?? 0
        };
      });

      const vehicleIds = Object.keys(vehicleLayers);
      renderSidebarButtons(vehicleIds);
      updateVehicleVisibility();

      const allSubLayers = [];

      Object.values(vehicleLayers).forEach(group => {
        group.eachLayer(layer => {
          if (layer.getBounds) {
            allSubLayers.push(layer);
          } else if (layer.getLatLng) {
            allSubLayers.push(L.featureGroup([layer]));
          }
        });
      });

      if (allSubLayers.length > 0) {
        const featureGroup = L.featureGroup(allSubLayers);
        map.fitBounds(featureGroup.getBounds());
      }
    })
    .catch(err => {
      alert("Load error: " + err.message);
      console.error(err);
    });
});