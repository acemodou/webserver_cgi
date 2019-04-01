var $ = function(id) {
    return document.getElementById(id);
}

var calculate_click = function() {
    var itemCost = parseFloat($("itemCost").value);
    var taxRate = parseFloat($("taxRate").value);

    if (isNaN(itemCost) || (itemCost < 0)) {
        alert("Item cost must be a number greater than 0!");
    } else if (isNaN(taxRate) || taxRate < 0) {
        alert("This must be a tax free state or the input is invalid! ");
    } else {
        var salestax = itemCost * (taxRate / 100);
        salestax = parseFloat(salestax.toFixed(2));

        var total = itemCost + salestax;

        $("salesTax").value = salestax;
        $("total").value = total.toFixed(2);
    }
}