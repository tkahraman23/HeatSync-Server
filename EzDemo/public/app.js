async function convert() {
    const type = document.getElementById("conversion").value;
    const valueStr = document.getElementById("value").value.trim();
    const resultEl = document.getElementById("result");

    if (valueStr === "" || isNaN(Number(valueStr))) {
        resultEl.textContent = "Lütfen geçerli bir sayý giriniz!";
        return;
    }

    try {
        const resp = await fetch(`/api/convert?type=${encodeURIComponent(type)}&value=${encodeURIComponent(valueStr)}`);
        const data = await resp.json();

        if (!data.ok) {
            resultEl.textContent = "Hata: " + (data.error || "Bilinmeyen hata");
            return;
        }

        let unit = "";
        if (type === "CtoF") unit = " °F";
        if (type === "FtoC") unit = " °C";
        if (type === "CtoK") unit = " K";
        if (type === "KtoC") unit = " °C";

        const n = Number(data.result);
        resultEl.textContent = "Sonuç: " + (Number.isInteger(n) ? n : n.toFixed(2)) + unit;
    } catch (e) {
        resultEl.textContent = "Að hatasý: " + e.message;
    }
}
