---
id: intro
title: Zoho OS Documentation
slug: /
---

<div className="landingHero">
  <div className="landingHero__grid">
    <div>
      <div className="eyebrow">Zoho OS Documentation</div>
      <h1>Technical documentation for the Zoho OS kernel.</h1>
      <p className="landingLead">
        This site documents the system from the real boot path upward, starting with the transition from
        GRUB handoff to <code>kmain()</code>. The goal is to make the codebase easier to understand,
        maintain, and extend.
      </p>

      <div className="landingActions">
        <a className="landingButton landingButton--primary" href="/boot/00-overview">
          View Boot Sequence
        </a>
        <a className="landingButton" href="/boot/09-64-bit-entry">
          View 64-bit Entry
        </a>
      </div>
    </div>

    <div className="statusPanel">
      <div className="statusPanel__title">Current Scope</div>
      <div className="statusRow">
        <span>Primary section</span>
        <strong>Boot sequence</strong>
      </div>
      <div className="statusRow">
        <span>Assembly sources</span>
        <strong><code>src/boot/*.asm</code></strong>
      </div>
      <div className="statusRow">
        <span>Kernel entry</span>
        <strong><code>src/kernel/main.c</code></strong>
      </div>
      <div className="statusRow">
        <span>Documentation model</span>
        <strong>Step-based technical reference</strong>
      </div>
    </div>
  </div>
</div>

<div className="featureGrid">
  <div className="featureCard">
    <div className="featureCard__kicker">Structure</div>
    <h2>Boot sequence first</h2>
    <p>
      The first section covers the most sensitive path in the system: the transition from GRUB’s
      32-bit environment into the 64-bit kernel.
    </p>
  </div>

  <div className="featureCard">
    <div className="featureCard__kicker">Sources</div>
    <h2>Linked to the implementation</h2>
    <p>
      Each page maps back to the current assembly and C sources so the documentation remains useful as an
      engineering reference.
    </p>
  </div>

  <div className="featureCard">
    <div className="featureCard__kicker">Expansion</div>
    <h2>Prepared for additional subsystems</h2>
    <p>
      The same format can be extended to GDT, IDT, PMM, VMM, scheduling, shell, and debugging topics
      without restructuring the site.
    </p>
  </div>
</div>

## Current coverage

- GRUB handoff and Multiboot2 entry assumptions
- Stack setup and bootstrap page table preparation
- `CR4.PAE`, `EFER.LME`, and `CR0.PG` transition points
- 64-bit GDT jump and the handoff to `kmain()`
- Common failure cases that lead to hangs or triple faults

## Next likely sections

- GDT and TSS layout
- IDT, PIC remap, and ISR entry flow
- PMM and VMM design
- Scheduler and shell internals
