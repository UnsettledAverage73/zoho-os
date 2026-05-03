const sidebars = {
  tutorialSidebar: [
    "intro",
    {
      type: "category",
      label: "Boot Sequence",
      items: [
        "boot/boot-00-overview",
        "boot/boot-01-grub-handoff",
        "boot/boot-02-multiboot-header",
        "boot/boot-03-entry-stack",
        "boot/boot-04-enable-pae",
        "boot/boot-05-page-tables-cr3",
        "boot/boot-06-enable-long-mode",
        "boot/boot-07-enable-paging",
        "boot/boot-08-gdt-far-jump",
        "boot/boot-09-64-bit-entry",
        "boot/boot-10-common-failures"
      ]
    }
  ]
};

module.exports = sidebars;
