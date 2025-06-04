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
  dayTile.addTo(map); // Initialize as Day mode
  
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


  // 可選：白色 overlay 仍可保留
  const mapPane = document.querySelector('.leaflet-map-pane');
  const overlayDiv = document.createElement('div');
  overlayDiv.className = 'tile-overlay';
  mapPane.appendChild(overlayDiv);

  const vehicleLayers = {};

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

      const allLayers = [];

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

        const group = L.layerGroup([lineLayer, pointLayer]).addTo(map);
        vehicleLayers[id] = group;

        lineLayer.eachLayer(l => allLayers.push(l));
        pointLayer.eachLayer(p => allLayers.push(p));

        const checkbox = document.createElement("input");
        checkbox.type = "checkbox";
        checkbox.id = `checkbox_${id}`;
        checkbox.checked = true;

        const label = document.createElement("label");
        label.htmlFor = checkbox.id;
        label.textContent = `Vehicle ${id}`;

        checkbox.addEventListener("change", () => {
          if (checkbox.checked) {
            group.addTo(map);
          } else {
            map.removeLayer(group);
          }
        });

        const container = document.getElementById("vehicle-list");
        container.appendChild(checkbox);
        container.appendChild(label);
        container.appendChild(document.createElement("br"));
      });

      const featureGroup = L.featureGroup(allLayers);
      map.fitBounds(featureGroup.getBounds());
    })

    .catch(err => {
      alert("Load error: " + err.message);
      console.error(err);
    });
});
