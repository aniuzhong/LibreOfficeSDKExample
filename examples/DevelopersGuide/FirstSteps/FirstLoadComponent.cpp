#include <bits/stdc++.h>
#include <libreoffice.hpp>

template <typename T>
using Ref = css::uno::Reference<T>; // note: css::uno::Reference should be allocated on stack

template <typename T>
using Seq = css::uno::Sequence<T>;

int main()
{
    try {
        // Get the remote office component context
        Ref<css::uno::XComponentContext> context = cppu::bootstrap();
        Ref<css::lang::XMultiComponentFactory> factory = context->getServiceManager();

        Ref<css::uno::XInterface> desktop = factory->createInstanceWithContext("com.sun.star.frame.Desktop", context);
        Ref<css::frame::XComponentLoader> loader(desktop, css::uno::UNO_QUERY);
        Seq<css::beans::PropertyValue> properties(0);
        Ref<css::lang::XComponent> component = loader->loadComponentFromURL("private:factory/scalc", "_blank", 0, properties);

        Ref<css::sheet::XSpreadsheetDocument> sheet_document(component, css::uno::UNO_QUERY);
        Ref<css::sheet::XSpreadsheets> sheets = sheet_document->getSheets();
        sheets->insertNewByName("MySheet", 0);
        css::uno::Type element_type = sheets->getElementType();
        std::cout << element_type.getTypeName().toUtf8().getStr() << '\n';
        Ref<css::sheet::XSpreadsheet> sheet(sheets->getByName("MySheet"), css::uno::UNO_QUERY);

        Ref<css::table::XCell> cell = sheet->getCellByPosition(0, 0);
        cell->setValue(21);
        cell = sheet->getCellByPosition(0, 1);
        cell->setValue(21);
        cell = sheet->getCellByPosition(0, 2);
        cell->setFormula("=sum(A1:A2)");

        Ref<css::beans::XPropertySet> cell_property(cell, css::uno::UNO_QUERY);
        cell_property->setPropertyValue("CellStyle", css::uno::Any(rtl::OUString("Result")));

        Ref<css::frame::XModel> model(component, css::uno::UNO_QUERY);
        Ref<css::frame::XController> controller = model->getCurrentController();
        Ref<css::sheet::XSpreadsheetView> sheet_view(controller, css::uno::UNO_QUERY);
        sheet_view->setActiveSheet(sheet);

        // *********************************************************
        // example for use of enum types
        cell_property->setPropertyValue("VertJustify", css::uno::Any(css::table::CellVertJustify::CellVertJustify_TOP));

        // *********************************************************
        // example for a sequence of PropertyValue structs
        // create an array with one PropertyValue struct, it contains references only
        properties = Seq<css::beans::PropertyValue>(1);

        // instantiate PropertyValue struct and set its member fields
        css::beans::PropertyValue as_template;
        as_template.Name = "AsTemplate";
        as_template.Value = css::uno::Any(true);

        // assign PropertyValue struct to array of references for PropertyValue structs
        properties[0] = as_template;

        // load calc file as template
        // component = loader->loadComponentFromURL("file:///c:/temp/DataAnalysys.ods", "_blank", 0, properties);

        // *********************************************************
        // example for use of XEnumerationAccess
        Ref<css::sheet::XCellRangesQuery> cell_query(sheet, css::uno::UNO_QUERY);
        Ref<css::sheet::XSheetCellRanges> formula_cells = cell_query->queryContentCells(css::sheet::CellFlags::FORMULA);
        Ref<css::container::XEnumerationAccess> formulas = formula_cells->getCells();
        Ref<css::container::XEnumeration> formula_enum = formulas->createEnumeration();
        while (formula_enum->hasMoreElements()) {
            Ref<css::table::XCell> cell(formula_enum->nextElement(), css::uno::UNO_QUERY);
            Ref<css::sheet::XCellAddressable> cell_address(cell, css::uno::UNO_QUERY);
            std::cout << "Formula cell in colomun "
                      << cell_address->getCellAddress().Column
                      << ", row " << cell_address->getCellAddress().Row
                      << " contains " << cell->getFormula() << '\n';
        }
    } catch (css::uno::Exception &e) {
        std::cout << e.Message << '\n';
        return 1;
    }

    return 0;
}
