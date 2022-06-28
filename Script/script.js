const temperatureItem = payload.find(item => item.variable === 'data');


if (temperatureItem) {
    const value = temperatureItem.value;
     var parts = value.split(":");
    const valor=parts[1];
    const new_var = valor.substring(0, valor.length - 1);
    const aux =parseFloat(new_var).toFixed(2);
    //context.log("valor separado ", new_var);

  // Then, we output the value and unit.
  temperatureItem.value = aux;
  temperatureItem.unit = 'C';
}